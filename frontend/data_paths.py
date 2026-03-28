"""Central resolver for data file paths.

Public files live in  data/
Private files live in data/private/
"""

from pathlib import Path

_ROOT    = Path(__file__).parent.parent
_PUBLIC  = _ROOT / 'data'
_PRIVATE = _ROOT / 'data' / 'private'

_PRIVATE_FILES = {
    'admins.csv', 'students.csv', 'teachers.csv', 'parents.csv',
    'fees.csv', 'attendance.csv', 'grades.csv',
    'notifications.csv', 'exam_results.csv',
}


def data_path(filename: str) -> Path:
    """Return the correct absolute path for a CSV file."""
    if filename in _PRIVATE_FILES:
        return _PRIVATE / filename
    return _PUBLIC / filename
