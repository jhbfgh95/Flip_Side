"""디버프 표시 테이블 추가. 기본 조회, --apply는 백업 후 적용. 기존 행은 보존합니다."""
import argparse
import datetime
import pathlib
import sqlite3

ROOT = pathlib.Path(__file__).resolve().parents[1]
ROWS = [
    (-1001, '실명', '', '공격이 적중하지 않습니다. 공격 및 적중 효과는 발동하지 않으며 기동 효과는 발동합니다. 보스는 패턴 피해만 차단되고 기믹은 실행됩니다.'),
    (-1002, '기절', '', '지속되는 동안 행동할 수 없습니다.'),
    (20001, '무기력 약화', '', '무기력이 감소합니다. 코인의 앞면과 뒷면 모두에 적용됩니다.'),
    (20002, '공격력 약화', '', '공격력이 감소합니다. 코인의 앞면과 뒷면 모두에 적용됩니다.'),
]

def snapshot(db):
    schema = db.execute("SELECT type,name,tbl_name,sql FROM sqlite_master WHERE tbl_name != 'debuff_definition' ORDER BY type,name").fetchall()
    data = {}
    for (name,) in db.execute("SELECT name FROM sqlite_master WHERE type='table' AND name != 'debuff_definition'"):
        quoted = '"' + name.replace('"', '""') + '"'
        data[name] = sorted(db.execute('SELECT * FROM ' + quoted).fetchall(), key=repr)
    return schema, data

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    path = ROOT / 'Content' / 'DB.db'
    with sqlite3.connect(path.as_uri() + '?mode=ro', uri=True) as source:
        before = snapshot(source)
        print('Existing table:', source.execute("SELECT sql FROM sqlite_master WHERE name='debuff_definition'").fetchall())
        print('Definitions:', ROWS)
        if not args.apply:
            return
        folder = ROOT / 'Saved' / 'DatabaseBackups'
        folder.mkdir(parents=True, exist_ok=True)
        backup = folder / ('DB_before_debuff_' + datetime.datetime.now().strftime('%Y%m%d_%H%M%S_%f') + '.db')
        with sqlite3.connect(backup) as target:
            source.backup(target)
    with sqlite3.connect(path) as db:
        db.execute('BEGIN IMMEDIATE')
        assert snapshot(db) == before, 'Database changed after backup; retry after inspection'
        fk_before = db.execute('PRAGMA foreign_key_check').fetchall()
        db.execute("CREATE TABLE IF NOT EXISTS debuff_definition (debuff_id INTEGER PRIMARY KEY CHECK(debuff_id != -1), debuff_name TEXT NOT NULL, icon_path TEXT NOT NULL DEFAULT '', debuff_description TEXT NOT NULL)")
        columns = [row[1] for row in db.execute('PRAGMA table_info(debuff_definition)')]
        assert columns == ['debuff_id', 'debuff_name', 'icon_path', 'debuff_description'], 'Existing schema differs; abort'
        for row in ROWS:
            old = db.execute('SELECT debuff_name FROM debuff_definition WHERE debuff_id=?', (row[0],)).fetchone()
            assert old is None or old[0] == row[1], 'ID already used for another definition'
            if old is None:
                db.execute('INSERT INTO debuff_definition VALUES (?,?,?,?)', row)
        assert snapshot(db) == before, 'Unrelated data or schema changed'
        assert db.execute('PRAGMA integrity_check').fetchone() == ('ok',)
        assert db.execute('PRAGMA foreign_key_check').fetchall() == fk_before
        print('Rows:', db.execute('SELECT * FROM debuff_definition ORDER BY debuff_id').fetchall())
    print('Backup:', backup)
    print('Committed; unrelated tables unchanged.')

if __name__ == '__main__':
    main()
