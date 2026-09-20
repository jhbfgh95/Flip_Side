"""승인된 아이템 기획 설명과 Instant 행만 반영. 기본 실행은 미리보기입니다."""
import argparse
import datetime
import pathlib
import sqlite3

ROOT = pathlib.Path(__file__).resolve().parents[1]
INSTANT = '아이템을 사용하면, 해당 시점에 바로 나타나는 효과입니다.'
# 상점 TextBlock과 공용이므로 DB에는 RichText 태그 없이 실제 줄바꿈을 저장합니다.
ITEMS = {
    1: ('보호막 융해물약', '필드 위 코인을 하나 선택합니다. \n선택한 코인이 보호막에 입히는 피해량을 3배 증가시킵니다.\n[적중]'),
    2: ('피의 갈망 물약', '필드 위 코인을 하나 선택합니다.\n선택한 코인이 입힌 총 피해량만큼 선택한 코인의 체력을 회복시킵니다.\n[적중]'),
    3: ('정화 물약', '\t필드 위 코인을 하나 선택합니다.\n\t선택한 코인의 모든 디버프를 제거합니다.\n\t[즉시]'),
    4: ('위상 변화 물약', '아직 행동하지 않은 필드 위 코인을 하나 선택합니다.\n선택한 코인의 면을 전환합니다. 현재 무기를 반대편 무기로 전환합니다.\n[즉시]'),
    5: ('융기 물약', '필드의 비어있는 칸을 선택합니다.\n선택한 칸에 장애물을 설치합니다. \n장애물은 다음 보스 공격 페이즈가 끝나면 사라집니다.\n[즉시]'),
    6: ('사방팔방 물약', '필드 위 코인을 하나 선택합니다.\n선택한 코인을 상하좌우로 인접한 빈칸 중 하나로 1칸 이동시킵니다.\n이때, 점유된 칸으로는 이동할 수 없습니다.\n[즉시]'),
}

def protected_snapshot(db):
    schema = db.execute('SELECT type,name,tbl_name,sql FROM sqlite_master ORDER BY type,name').fetchall()
    data = {}
    for (name,) in db.execute("SELECT name FROM sqlite_master WHERE type='table'"):
        table = '"' + name.replace('"', '""') + '"'
        columns = [r[1] for r in db.execute(f'PRAGMA table_info({table})')]
        if name == 'item':
            columns.remove('item_description')
        selection = ','.join('"' + c.replace('"', '""') + '"' for c in columns)
        where = " WHERE keyword_code != 'Instant'" if name == 'keyword_definition' else ''
        data[name] = sorted(db.execute(f'SELECT {selection} FROM {table}{where}').fetchall(), key=repr)
    return schema, data

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--apply', action='store_true')
    parser.add_argument('--descriptions-only', action='store_true')
    args = parser.parse_args()
    path = ROOT / 'Content/DB.db'
    with sqlite3.connect(path.as_uri() + '?mode=ro', uri=True) as read:
        for identity, (name, description) in ITEMS.items():
            assert read.execute('SELECT item_name FROM item WHERE item_id=?', (identity,)).fetchone() == (name,)
            print(f'{identity}: {name}\n{description}\n')
        if not args.apply:
            return
        folder = ROOT / 'Saved/Backups'
        folder.mkdir(parents=True, exist_ok=True)
        backup = folder / ('DB_before_item_descriptions_' + datetime.datetime.now().strftime('%Y%m%d_%H%M%S_%f') + '.db')
        with sqlite3.connect(backup) as destination:
            read.backup(destination)
    with sqlite3.connect(path) as db, sqlite3.connect(backup) as before:
        db.execute('BEGIN IMMEDIATE')
        assert list(db.iterdump()) == list(before.iterdump()), 'DB changed since backup; retry after review'
        for identity, (_, description) in ITEMS.items():
            db.execute('UPDATE item SET item_description=? WHERE item_id=?', (description, identity))
        existing = db.execute("SELECT keyword_id FROM keyword_definition WHERE keyword_code='Instant'").fetchone()
        if args.descriptions_only:
            pass
        elif existing:
            # 팀원이 지정한 아이콘/색상/정렬은 재실행 시에도 보존합니다.
            db.execute("UPDATE keyword_definition SET display_name_ko='즉시',description_ko=? WHERE keyword_code='Instant'", (INSTANT,))
        else:
            identity, order = db.execute('SELECT COALESCE(MAX(keyword_id),0)+1,COALESCE(MAX(sort_order),0)+1 FROM keyword_definition').fetchone()
            db.execute('INSERT INTO keyword_definition (keyword_id,keyword_code,display_name_ko,description_ko,ui_color_rgba,is_enabled,sort_order,icon_path) VALUES (?,?,?,?,?,?,?,?)', (identity, 'Instant', '즉시', INSTANT, 'FFFFFFFF', 1, order, ''))
        assert protected_snapshot(db) == protected_snapshot(before), 'Unexpected schema or unrelated data change'
        if args.descriptions_only:
            assert db.execute('SELECT * FROM keyword_definition ORDER BY keyword_id').fetchall() == before.execute('SELECT * FROM keyword_definition ORDER BY keyword_id').fetchall()
        other_before = [(i, d) for i, d in before.execute('SELECT item_id,item_description FROM item ORDER BY item_id') if i not in ITEMS]
        other_after = [(i, d) for i, d in db.execute('SELECT item_id,item_description FROM item ORDER BY item_id') if i not in ITEMS]
        assert other_before == other_after
        assert db.execute('PRAGMA integrity_check').fetchone() == ('ok',)
        assert db.execute('PRAGMA foreign_key_check').fetchall() == before.execute('PRAGMA foreign_key_check').fetchall()
        db.commit()
        print(f'Updated 6 item descriptions; descriptions_only={args.descriptions_only}; unrelated data unchanged. Backup: {backup}')

if __name__ == '__main__':
    main()
