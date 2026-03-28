"""
Seed script for Student Management System.
Generates all CSV data for classes 1A through 6C.
Run from the project root: python seed.py
"""

import csv
import os
import random
from datetime import date, timedelta

# ── FNV-1a hash (matches C++ Crypto::hashPassword) ────────────────────────────
def fnv1a(password):
    FNV_PRIME  = 1099511628211
    FNV_OFFSET = 14695981039346656037
    h = FNV_OFFSET
    for c in password.encode():
        h ^= c
        h *= FNV_PRIME
        h = h % (2**64)
    return format(h, 'x')

# ── Constants ─────────────────────────────────────────────────────────────────
DATA_DIR = "data"

GRADES   = [1, 2, 3, 4, 5, 6]
SECTIONS = ['A', 'B', 'C']
CLASSES  = [f"{g}{s}" for g in GRADES for s in SECTIONS]   # 1A..6C

SUBJECTS = ['Mathematics', 'English', 'Science', 'Social Studies', 'ICT',
            'French',      'History', 'Physics', 'Biology',        'Chemistry']

# Fee per grade level
FEE_MAP = {1: 800.00, 2: 900.00, 3: 1000.00,
           4: 1200.00, 5: 1500.00, 6: 1800.00}

DAYS = ['Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday']

# Ghanaian first/last names pool
FIRST_NAMES = [
    'Kwame','Ama','Kofi','Akua','Yaw','Abena','Kojo','Adwoa','Kwesi','Efua',
    'Fiifi','Akosua','Ato','Maame','Kobbina','Afia','Kwabena','Adjoa','Nana',
    'Serwaa','Isaac','Priscilla','Elijah','Dorcas','Aaron','Eunice','Moses',
    'Phoebe','Gideon','Tabitha','James','Mary','Peter','Sarah','David','Ruth',
    'Paul','Esther','Mark','Lydia','Stephen','Hannah','Joseph','Naomi',
    'Daniel','Rebecca','Andrew','Deborah','Philip','Judith','Thomas','Rachel',
    'Simon','Leah','Joshua','Miriam','Caleb','Abigail','Samuel','Grace',
    'Eric','Linda','Felix','Diana','Benjamin','Esi','Kojo','Adwoa','Nii',
    'Efua','Kwesi','Abena','Fiifi','Akosua','Ato','Maame','Kobbina','Afia',
    'John','Amina','Kwame','Linda','Grace','Samuel','Abena','Kofi','Diana'
]

LAST_NAMES = [
    'Mensah','Boateng','Asante','Ofori','Tetteh','Adjei','Darko','Owusu',
    'Agyeman','Appiah','Osei','Asare','Frimpong','Quaye','Antwi','Sarpong',
    'Acheampong','Amoah','Bonsu','Danso','Gyasi','Kumi','Larbi','Manu',
    'Nkrumah','Obeng','Poku','Quartey','Rockson','Sarkodie','Twum','Yeboah'
]

TEACHER_DATA = [
    # (id, username, password, name, subject, department)
    (1,  't.ali',    'teach001', 'Mr. Ali Hassan',    'Mathematics', 'Science'),
    (2,  't.sara',   'teach002', 'Ms. Sara Khan',     'English',     'Languages'),
    (3,  't.james',  'teach003', 'Mr. James Osei',    'Physics',     'Science'),
    (4,  't.fatima', 'teach004', 'Ms. Fatima Noor',   'Biology',     'Science'),
    (5,  't.david',  'teach005', 'Mr. David Mensah',  'History',     'Humanities'),
    (6,  't.kweku',  'teach006', 'Mr. Kweku Asante',  'Chemistry',   'Science'),
    (7,  't.ama',    'teach007', 'Ms. Ama Boateng',   'ICT',         'Technology'),
    (8,  't.kofi',   'teach008', 'Mr. Kofi Darko',    'French',      'Languages'),
    (9,  't.grace',  'teach009', 'Ms. Grace Adjei',   'Science',     'Science'),
    (10, 't.samuel', 'teach010', 'Mr. Samuel Quaye',  'Social Studies', 'Humanities'),
]

# Subject → teacher mapping
SUBJECT_TEACHER = {
    'Mathematics':   (1,  'Mr. Ali Hassan'),
    'English':       (2,  'Ms. Sara Khan'),
    'Physics':       (3,  'Mr. James Osei'),
    'Biology':       (4,  'Ms. Fatima Noor'),
    'History':       (5,  'Mr. David Mensah'),
    'Chemistry':     (6,  'Mr. Kweku Asante'),
    'ICT':           (7,  'Ms. Ama Boateng'),
    'French':        (8,  'Mr. Kofi Darko'),
    'Science':       (9,  'Ms. Grace Adjei'),
    'Social Studies':(10, 'Mr. Samuel Quaye'),
}

# 5 subjects per grade
GRADE_SUBJECTS = {
    1: ['Mathematics', 'English', 'Science', 'Social Studies', 'ICT'],
    2: ['Mathematics', 'English', 'Science', 'Social Studies', 'French'],
    3: ['Mathematics', 'English', 'Science', 'History',        'ICT'],
    4: ['Mathematics', 'English', 'Physics', 'Biology',        'History'],
    5: ['Mathematics', 'English', 'Physics', 'Biology',        'Chemistry'],
    6: ['Mathematics', 'English', 'Physics', 'Biology',        'Chemistry'],
}

random.seed(42)

def random_name():
    return f"{random.choice(FIRST_NAMES)} {random.choice(LAST_NAMES)}"

def date_range(start: date, days: int):
    return [start + timedelta(days=i) for i in range(days)]

def weekdays_in_range(start: date, end: date):
    d, result = start, []
    while d <= end:
        if d.weekday() < 5:
            result.append(d)
        d += timedelta(days=1)
    return result

# ── 1. fee_structure.csv ──────────────────────────────────────────────────────
def seed_fee_structure():
    rows = []
    for g in GRADES:
        for s in SECTIONS:
            rows.append([f"{g}{s}", f"{FEE_MAP[g]:.6f}"])
    # Keep existing 12A/12B
    rows.append(['12A', '2000.000000'])
    rows.append(['12B', '2000.000000'])
    with open(f"{DATA_DIR}/fee_structure.csv", 'w', newline='') as f:
        csv.writer(f).writerows(rows)
    print(f"  fee_structure.csv — {len(rows)} rows")

# ── 2. teachers.csv ───────────────────────────────────────────────────────────
def seed_teachers():
    # Build assigned classes per subject teacher
    assigned = {tid: [] for tid, *_ in TEACHER_DATA}
    for g in GRADES:
        for s in SECTIONS:
            cls = f"{g}{s}"
            for subj in GRADE_SUBJECTS[g]:
                tid = SUBJECT_TEACHER[subj][0]
                if cls not in assigned[tid]:
                    assigned[tid].append(cls)

    rows = []
    for tid, uname, pwd, name, subj, dept in TEACHER_DATA:
        classes_str = '|'.join(assigned[tid])
        rows.append([tid, uname, fnv1a(pwd), name, subj, dept, classes_str])

    with open(f"{DATA_DIR}/teachers.csv", 'w', newline='') as f:
        csv.writer(f).writerows(rows)
    print(f"  teachers.csv — {len(rows)} teachers")

# ── 3. students.csv + parents.csv ─────────────────────────────────────────────
def seed_students_parents():
    student_rows = []
    parent_rows  = []
    sid = 1
    pid_start = 1000   # parent IDs start at 1000 to avoid collision

    # Track used names to avoid duplicates
    used_names = set()

    for g in GRADES:
        for s in SECTIONS:
            cls = f"{g}{s}"
            count = random.randint(12, 15)
            for i in range(count):
                # Unique name
                name = random_name()
                attempts = 0
                while name in used_names and attempts < 50:
                    name = random_name()
                    attempts += 1
                used_names.add(name)

                roll = f"R{sid:03d}"
                uname = f"s.{name.split()[0].lower()}{sid}"
                pwd   = f"stu{sid:03d}"
                parent_id = pid_start

                student_rows.append([sid, uname, fnv1a(pwd), name, roll, cls, parent_id])

                # Parent
                p_name  = f"Mr./Mrs. {name.split()[1]}"
                p_uname = f"p.{name.split()[1].lower()}{sid}"
                p_pwd   = f"par{sid:03d}"
                parent_rows.append([parent_id, p_uname, fnv1a(p_pwd), p_name, sid])

                sid       += 1
                pid_start += 1

    with open(f"{DATA_DIR}/students.csv", 'w', newline='') as f:
        csv.writer(f).writerows(student_rows)
    with open(f"{DATA_DIR}/parents.csv", 'w', newline='') as f:
        csv.writer(f).writerows(parent_rows)
    print(f"  students.csv  — {len(student_rows)} students")
    print(f"  parents.csv   — {len(parent_rows)} parents")
    return student_rows

# ── 4. timetable.csv ──────────────────────────────────────────────────────────
def seed_timetable():
    slots = [
        ('07:30', '08:30'),
        ('08:30', '09:30'),
        ('10:00', '11:00'),
        ('11:00', '12:00'),
        ('13:00', '14:00'),
    ]
    rows = []
    for g in GRADES:
        for s in SECTIONS:
            cls     = f"{g}{s}"
            subjects = GRADE_SUBJECTS[g]
            for day in DAYS:
                for slot_idx, (start, end) in enumerate(slots[:len(subjects)]):
                    subj    = subjects[slot_idx % len(subjects)]
                    teacher = SUBJECT_TEACHER[subj][1]
                    rows.append([cls, day, start, end, subj, teacher])

    with open(f"{DATA_DIR}/timetable.csv", 'w', newline='') as f:
        csv.writer(f).writerows(rows)
    print(f"  timetable.csv — {len(rows)} slots")

# ── 5. courses.csv ────────────────────────────────────────────────────────────
def seed_courses():
    rows = []
    cid  = 1
    for g in GRADES:
        for s in SECTIONS:
            cls = f"{g}{s}"
            for subj in GRADE_SUBJECTS[g]:
                tid, tname = SUBJECT_TEACHER[subj]
                code    = f"{subj[:4].upper()}{g}{s}"
                name    = f"{subj} {g}{s}"
                credits = 4 if subj in ('Mathematics', 'Physics', 'Chemistry') else 3
                rows.append([cid, code, name, subj, cls, tname, credits])
                cid += 1

    with open(f"{DATA_DIR}/courses.csv", 'w', newline='') as f:
        csv.writer(f).writerows(rows)
    print(f"  courses.csv   — {len(rows)} courses")

# ── 6. attendance.csv ─────────────────────────────────────────────────────────
def seed_attendance(student_rows):
    term_start = date(2025, 4, 7)
    term_end   = date(2025, 6, 27)
    school_days = weekdays_in_range(term_start, term_end)

    rows = []
    for sid, uname, pwd_hash, name, roll, cls, parent_id in student_rows:
        for d in school_days:
            present = 1 if random.random() > 0.1 else 0
            rows.append([sid, name, cls, d.strftime('%Y-%m-%d'), present])

    with open(f"{DATA_DIR}/attendance.csv", 'w', newline='') as f:
        csv.writer(f).writerows(rows)
    print(f"  attendance.csv — {len(rows)} records")

# ── 7. grades.csv ─────────────────────────────────────────────────────────────
def seed_grades(student_rows):
    rows = []
    for sid, uname, pwd_hash, name, roll, cls, parent_id in student_rows:
        g = int(cls[:-1])
        subjects = GRADE_SUBJECTS[g]
        for term in ['Term1', 'Term2']:
            for subj in subjects:
                marks = round(random.uniform(40, 98), 2)
                rows.append([sid, name, subj, term, f"{marks:.6f}", '100.000000'])

    with open(f"{DATA_DIR}/grades.csv", 'w', newline='') as f:
        csv.writer(f).writerows(rows)
    print(f"  grades.csv    — {len(rows)} records")

# ── 8. fees.csv ───────────────────────────────────────────────────────────────
def seed_fees(student_rows):
    rows = []
    inv_id = 1
    due_date = '2025-04-30'
    for sid, uname, pwd_hash, name, roll, cls, parent_id in student_rows:
        g      = int(cls[:-1])
        amount = FEE_MAP[g]
        paid   = round(random.uniform(0, amount), 2)
        settled = 1 if paid >= amount else 0
        paid_date = '2025-04-15' if paid > 0 else ''
        rows.append([inv_id, sid, name, cls, due_date, paid_date,
                     f"{amount:.6f}", f"{paid:.6f}", settled])
        inv_id += 1

    with open(f"{DATA_DIR}/fees.csv", 'w', newline='') as f:
        csv.writer(f).writerows(rows)
    print(f"  fees.csv      — {len(rows)} invoices")

# ── 9. assignments.csv ────────────────────────────────────────────────────────
def seed_assignments(student_rows):
    # Get unique classes
    classes_seen = []
    for row in student_rows:
        cls = row[5]
        if cls not in classes_seen:
            classes_seen.append(cls)

    rows = []
    aid  = 1
    due  = date(2025, 4, 21)
    for cls in classes_seen:
        g        = int(cls[:-1])
        subjects = GRADE_SUBJECTS[g]
        for subj in subjects:
            tid     = SUBJECT_TEACHER[subj][0]
            title   = f"{subj} Assignment {aid}"
            fileref = f"{subj[:3].lower()}_hw{aid}.pdf"
            rows.append([aid, tid, title, subj, due.strftime('%Y-%m-%d'), fileref, cls])
            aid += 1
            due += timedelta(days=2)

    with open(f"{DATA_DIR}/assignments.csv", 'w', newline='') as f:
        csv.writer(f).writerows(rows)
    print(f"  assignments.csv — {len(rows)} assignments")

# ── 10. exams.csv ─────────────────────────────────────────────────────────────
def seed_exams(student_rows):
    classes_seen = []
    for row in student_rows:
        cls = row[5]
        if cls not in classes_seen:
            classes_seen.append(cls)

    rows = []
    eid  = 1
    for cls in classes_seen:
        g        = int(cls[:-1])
        subjects = GRADE_SUBJECTS[g]
        for i, subj in enumerate(subjects):
            exam_date = date(2025, 4, 28) + timedelta(days=i)
            rows.append([eid, f"Mid-Term {subj}", 'Midterm', cls, subj,
                         exam_date.strftime('%Y-%m-%d'), '100.000000'])
            eid += 1

    with open(f"{DATA_DIR}/exams.csv", 'w', newline='') as f:
        csv.writer(f).writerows(rows)
    print(f"  exams.csv     — {len(rows)} exams")

# ── 11. notifications.csv ─────────────────────────────────────────────────────
def seed_notifications(student_rows):
    rows = []
    nid  = 1
    today = date.today().strftime('%Y-%m-%d')
    for sid, uname, pwd_hash, name, roll, cls, parent_id in student_rows:
        g    = int(cls[:-1])
        fee  = FEE_MAP[g]
        subj = ', '.join(GRADE_SUBJECTS[g])
        msg  = (f"Welcome to {cls}! Your termly fee is GHS {fee:.2f} (due 2025-04-30). "
                f"Your subjects: {subj}. Roll No: {roll}. "
                f"Login with username '{uname}' to view your timetable and assignments.")
        rows.append([nid, sid, msg, today, 'Welcome', 0])
        nid += 1

    with open(f"{DATA_DIR}/notifications.csv", 'w', newline='') as f:
        csv.writer(f).writerows(rows)
    print(f"  notifications.csv — {len(rows)} notifications")

# ── 12. calendar.csv ──────────────────────────────────────────────────────────
def seed_calendar():
    rows = [
        [1,  'Term 1 Begins',          '2025-01-06', '2025-01-06', 'Academic', 'First term commences for all classes'],
        [2,  'Mid-Term Exams T1',       '2025-02-17', '2025-02-21', 'Exam',     'Mid-term examinations Term 1'],
        [3,  'Mid-Term Break T1',       '2025-02-24', '2025-02-28', 'Holiday',  'Mid-term break'],
        [4,  'End of Term 1 Exams',     '2025-03-24', '2025-03-28', 'Exam',     'End of term 1 examinations'],
        [5,  'Term 1 Ends',             '2025-04-04', '2025-04-04', 'Academic', 'Last day of first term'],
        [6,  'Term 2 Begins',           '2025-04-07', '2025-04-07', 'Academic', 'Second term commences for all classes'],
        [7,  'Mid-Term Exams T2',       '2025-04-28', '2025-05-02', 'Exam',     'Mid-term examinations Term 2'],
        [8,  'Mid-Term Break T2',       '2025-05-05', '2025-05-09', 'Holiday',  'Mid-term break'],
        [9,  'Sports Day',              '2025-05-10', '2025-05-10', 'Sports',   'Annual inter-class sports competition'],
        [10, 'Parent-Teacher Meeting',  '2025-05-16', '2025-05-16', 'Meeting',  'Parents meet class teachers'],
        [11, 'End of Term 2 Exams',     '2025-06-09', '2025-06-13', 'Exam',     'End of term 2 examinations'],
        [12, 'Prize Giving Day',        '2025-06-20', '2025-06-20', 'Academic', 'Annual prize giving ceremony'],
        [13, 'Term 2 Ends',             '2025-06-27', '2025-06-27', 'Academic', 'Last day of second term'],
        [14, 'Long Vacation',           '2025-06-28', '2025-08-31', 'Holiday',  'Long vacation between Term 2 and Term 3'],
        [15, 'Term 3 Begins',           '2025-09-01', '2025-09-01', 'Academic', 'Third term commences for all classes'],
        [16, 'Mid-Term Exams T3',       '2025-10-13', '2025-10-17', 'Exam',     'Mid-term examinations Term 3'],
        [17, 'Mid-Term Break T3',       '2025-10-20', '2025-10-24', 'Holiday',  'Mid-term break'],
        [18, 'End of Term 3 Exams',     '2025-11-24', '2025-11-28', 'Exam',     'End of term 3 examinations'],
        [19, 'Prize Giving & Graduation','2025-12-05','2025-12-05', 'Academic', 'Prize giving and Form 6 graduation'],
        [20, 'Term 3 Ends',             '2025-12-12', '2025-12-12', 'Academic', 'Last day of third term'],
    ]
    with open(f"{DATA_DIR}/calendar.csv", 'w', newline='') as f:
        csv.writer(f).writerows(rows)
    print(f"  calendar.csv  — {len(rows)} events")

# ── 13. announcements.csv ─────────────────────────────────────────────────────
def seed_announcements():
    rows = [
        [1, 'Welcome Back',           'Welcome to the new term. Classes begin Monday.',                          '2025-04-07', 'System Admin', 0, 1],
        [2, 'Exam Timetable Released','Mid-term exams start April 28. Check the notice board.',                  '2025-04-10', 'System Admin', 0, 1],
        [3, 'Fee Reminder',           'Outstanding fees must be cleared by April 30.',                           '2025-04-12', 'System Admin', 0, 1],
        [4, 'Sports Day Notice',      'Annual sports day is scheduled for May 10. All students must participate.','2025-04-14', 'Mr. Ali Hassan', 3, 1],
        [5, 'Library Hours Extended', 'Library now open until 5pm on weekdays.',                                 '2025-04-15', 'System Admin', 0, 1],
        [6, 'Parent-Teacher Meeting', 'PTM scheduled for May 16. Parents are encouraged to attend.',             '2025-04-20', 'System Admin', 0, 1],
        [7, 'New ICT Lab Open',       'The new ICT lab is now open for student use during free periods.',        '2025-04-22', 'Ms. Ama Boateng', 0, 1],
    ]
    with open(f"{DATA_DIR}/announcements.csv", 'w', newline='') as f:
        csv.writer(f).writerows(rows)
    print(f"  announcements.csv — {len(rows)} announcements")

# ── 14. materials.csv ─────────────────────────────────────────────────────────
def seed_materials(student_rows):
    classes_seen = []
    for row in student_rows:
        cls = row[5]
        if cls not in classes_seen:
            classes_seen.append(cls)

    rows = []
    mid = 1
    for cls in classes_seen:
        g        = int(cls[:-1])
        subjects = GRADE_SUBJECTS[g]
        for subj in subjects:
            tid   = SUBJECT_TEACHER[subj][0]
            title = f"{subj} Notes - {cls}"
            path  = f"{subj[:3].lower()}_notes_{cls.lower()}.pdf"
            rows.append([mid, tid, title, subj, path, cls])
            mid += 1

    with open(f"{DATA_DIR}/materials.csv", 'w', newline='') as f:
        csv.writer(f).writerows(rows)
    print(f"  materials.csv — {len(rows)} materials")

# ── 15. books.csv ─────────────────────────────────────────────────────────────
def seed_books():
    books = [
        'Mathematics for Senior High School',
        'Comprehensive English Grammar',
        'Physics: Principles and Problems',
        'Biology: Life on Earth',
        'Chemistry in Context',
        'ICT Fundamentals',
        'French for Beginners',
        'History of West Africa',
        'Science for Junior High',
        'Social Studies Textbook',
        'Advanced Mathematics',
        'Literature in English',
        'Integrated Science',
        'Core Mathematics',
        'Elective Mathematics',
    ]
    authors = [
        'Asante & Mensah', 'Boateng et al.', 'Osei & Khan',
        'Noor & Adjei',    'Hassan & Darko', 'Ama Boateng',
        'Kofi Darko',      'David Mensah',   'Grace Adjei',
        'Samuel Quaye',    'Ali Hassan',      'Sara Khan',
        'James Osei',      'Fatima Noor',     'Kweku Asante',
    ]
    rows = []
    for i, (title, author) in enumerate(zip(books, authors), 1):
        rows.append([i, title, author, 1, 0, '', '', ''])

    with open(f"{DATA_DIR}/books.csv", 'w', newline='') as f:
        csv.writer(f).writerows(rows)
    print(f"  books.csv     — {len(rows)} books")

# ── 16. exam_results.csv ──────────────────────────────────────────────────────
def seed_exam_results(student_rows):
    # Load exams to get IDs
    exams = []
    with open(f"{DATA_DIR}/exams.csv", 'r') as f:
        for row in csv.reader(f):
            if len(row) >= 7:
                exams.append({'id': int(row[0]), 'title': row[1],
                              'subject': row[4], 'class': row[3],
                              'total': float(row[6])})

    # Map class → exams
    class_exams = {}
    for e in exams:
        class_exams.setdefault(e['class'], []).append(e)

    rows = []
    rid  = 1
    for sid, uname, pwd_hash, name, roll, cls, parent_id in student_rows:
        for e in class_exams.get(cls, []):
            marks = round(random.uniform(35, 98), 2)
            rows.append([rid, e['id'], sid, name, e['title'],
                         e['subject'], cls, f"{marks:.6f}", f"{e['total']:.6f}"])
            rid += 1

    with open(f"{DATA_DIR}/exam_results.csv", 'w', newline='') as f:
        csv.writer(f).writerows(rows)
    print(f"  exam_results.csv — {len(rows)} results")

# ── Main ──────────────────────────────────────────────────────────────────────
if __name__ == '__main__':
    os.makedirs(DATA_DIR, exist_ok=True)
    print("Seeding data...")

    seed_fee_structure()
    seed_teachers()
    student_rows = seed_students_parents()
    seed_timetable()
    seed_courses()
    seed_attendance(student_rows)
    seed_grades(student_rows)
    seed_fees(student_rows)
    seed_assignments(student_rows)
    seed_exams(student_rows)
    seed_notifications(student_rows)
    seed_calendar()
    seed_announcements()
    seed_materials(student_rows)
    seed_books()
    seed_exam_results(student_rows)

    print("\nDone! All CSV files seeded successfully.")
    print(f"Total students: {len(student_rows)}")
    print(f"Classes: {', '.join(CLASSES)}")
