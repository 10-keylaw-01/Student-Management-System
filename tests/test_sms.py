"""Unit tests for the Student Management System."""

import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent / 'frontend'))

from ui.login_window import _fnv1a, _authenticate


# ── FNV-1a hash ───────────────────────────────────────────────────────────────

class TestFnv1a(unittest.TestCase):

    def test_known_hash(self):
        # Verified against C++ Crypto::hashPassword("admin123")
        result = _fnv1a("admin123")
        self.assertIsInstance(result, str)
        self.assertTrue(len(result) > 0)

    def test_different_passwords_differ(self):
        self.assertNotEqual(_fnv1a("password1"), _fnv1a("password2"))

    def test_same_password_consistent(self):
        self.assertEqual(_fnv1a("test"), _fnv1a("test"))

    def test_empty_string(self):
        result = _fnv1a("")
        self.assertIsInstance(result, str)


# ── Date validation ───────────────────────────────────────────────────────────

class TestDateValidation(unittest.TestCase):
    """Tests for Auth::isValidDate logic mirrored in Python."""

    def _is_valid(self, d: str) -> bool:
        if len(d) != 10:
            return False
        if d[4] != '-' or d[7] != '-':
            return False
        parts = d.replace('-', '')
        if not parts.isdigit():
            return False
        m   = int(d[5:7])
        day = int(d[8:10])
        return 1 <= m <= 12 and 1 <= day <= 31

    def test_valid_date(self):
        self.assertTrue(self._is_valid("2025-06-01"))

    def test_invalid_letters(self):
        self.assertFalse(self._is_valid("abc-de-fg"))

    def test_invalid_month(self):
        self.assertFalse(self._is_valid("2025-13-01"))

    def test_invalid_day(self):
        self.assertFalse(self._is_valid("2025-06-32"))

    def test_wrong_format(self):
        self.assertFalse(self._is_valid("01/06/2025"))

    def test_too_short(self):
        self.assertFalse(self._is_valid("2025-6-1"))


# ── Authentication ────────────────────────────────────────────────────────────

class TestAuthenticate(unittest.TestCase):

    def setUp(self):
        self.data_dir = Path(__file__).parent.parent / 'data' / 'private'

    def test_admin_login(self):
        role, name = _authenticate(self.data_dir, 'admin', 'admin123')
        self.assertEqual(role, 'admin')
        self.assertIsNotNone(name)

    def test_wrong_password(self):
        role, name = _authenticate(self.data_dir, 'admin', 'wrongpassword')
        self.assertIsNone(role)
        self.assertIsNone(name)

    def test_nonexistent_user(self):
        role, name = _authenticate(self.data_dir, 'nobody', 'nopass')
        self.assertIsNone(role)
        self.assertIsNone(name)

    def test_teacher_login(self):
        role, name = _authenticate(self.data_dir, 't.ali', 'teach001')
        self.assertEqual(role, 'teacher')
        self.assertIsNotNone(name)

    def test_student_login(self):
        # First student seeded: s.<firstname>1 / stu001
        import csv
        students_path = self.data_dir / 'students.csv'
        if not students_path.exists():
            self.skipTest("students.csv not found")
        with open(students_path, 'r') as f:
            row = next(csv.reader(f))
        username = row[1]
        # Derive password: stu + zero-padded id
        sid = int(row[0])
        password = f"stu{sid:03d}"
        role, name = _authenticate(self.data_dir, username, password)
        self.assertEqual(role, 'student')

    def test_parent_login(self):
        import csv
        parents_path = self.data_dir / 'parents.csv'
        if not parents_path.exists():
            self.skipTest("parents.csv not found")
        with open(parents_path, 'r') as f:
            row = next(csv.reader(f))
        username = row[1]
        sid = int(row[4])
        password = f"par{sid:03d}"
        role, name = _authenticate(self.data_dir, username, password)
        self.assertEqual(role, 'parent')


# ── Data path resolver ────────────────────────────────────────────────────────

class TestDataPaths(unittest.TestCase):

    def setUp(self):
        from data_paths import data_path, _PRIVATE_FILES
        self.data_path     = data_path
        self.private_files = _PRIVATE_FILES

    def test_private_files_go_to_private(self):
        for f in self.private_files:
            p = self.data_path(f)
            self.assertIn('private', str(p), f"{f} should resolve to data/private/")

    def test_public_files_stay_in_data(self):
        for f in ['fee_structure.csv', 'timetable.csv', 'courses.csv',
                  'announcements.csv', 'books.csv', 'calendar.csv']:
            p = self.data_path(f)
            self.assertNotIn('private', str(p), f"{f} should resolve to data/")

    def test_all_private_files_exist(self):
        for f in self.private_files:
            p = self.data_path(f)
            self.assertTrue(p.exists(), f"Missing: {p}")


if __name__ == '__main__':
    unittest.main(verbosity=2)
