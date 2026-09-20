"""무기 기획서 + 승인된 Stat 표기 규칙을 KOR_DES에만 적용합니다."""
import argparse
import datetime
import pathlib
import sqlite3

ROOT = pathlib.Path(__file__).resolve().parents[1]

def stat(expression):
    return f"<Stat>{expression}</Stat>"

A = stat('[STAT:AttackPower]')
W = stat('[STAT:WeaponPower]')
C = stat('[STAT:Count]')
ATTACK = f'[KW:Attack]\n[STAT:AttackRange] 내 적 하나에게 {A}만큼 피해를 줍니다.'

# 이름은 DB 원본 그대로 검증하며, 설명 문장만 기획서에 맞춥니다.
DESCRIPTIONS = {
    1: ('쇠파이프', '[KW:Attack]\n[STAT:AttackRange] 내 적 하나에게 ' + stat('[STAT:AttackPower] + [STAT:WeaponPower]') + '만큼 피해를 줍니다.'),
    2: ('증기 체인톱', f'[KW:Attack] [KW:Continuous] : {W}\n[STAT:AttackRange] 내 적에게 {A}만큼 피해를 줍니다.'),
    3: ('버거', ATTACK + f'\n[KW:Hit]\n다음 턴 공격 시 코인 스탯의 공격력이 {W}만큼 증가합니다.\n이 효과는 최대 3회까지 중첩됩니다. 보스에게 피해를 주지 못하면 모든 중첩을 잃습니다.'),
    4: ('수혈 동력 혈사포', '[KW:Attack]\n[KW:Absorb] 후에, [STAT:AttackRange] 내 적에게 ' + stat('[STAT:AttackPower] + [BUFF:Absorb]') + f'만큼 피해를 줍니다.\n[KW:Mobility] [KW:Absorb]\n[STAT:AbilityRange] 안에 있는 모든 코인의 체력을 {W}만큼 흡수합니다.'),
    5: ('자동 포탑 설치 세트', ATTACK + f'\n[KW:Mobility]\n[STAT:AbilityRange] 내에 포탑을 설치합니다.\n포탑은 이 무기와 같은 [STAT:AttackRange]를 가지며, {W}을 공격력으로 갖습니다.'),
    6: ('거치형 스나이퍼 라이플', ATTACK + f'\n[KW:Hit]\n이 무기가 장애물과 인접해 있다면, 무기와 적까지의 거리 1칸당 {W}만큼 추가 피해를 줍니다.\n추가 피해: {W} × 무기와 적까지의 거리'),
    7: ('급속 냉각기', ATTACK + '\n[KW:Hit]\n' + stat('[STAT:WeaponPower] * 10') + '%의 확률로 적을 1턴 동안 기절시킵니다.'),
    8: ('연막 슈트', f'[KW:Attack]\n[STAT:AttackRange] 내의 적에게 {A}만큼 피해를 입힙니다.\n[KW:Mobility]\n[STAT:AbilityRange] 내 코인이 피해를 받을 때, ' + stat('[STAT:WeaponPower] * 10') + '%의 확률로 해당 피해를 받지 않습니다.\n[KW:Hit]\n[KW:Mobility] 능력의 피해를 받지 않을 확률이 ' + stat('([STAT:WeaponPower] + [STAT:AttackPower]) * 10') + '%로 증가합니다.'),
    9: ('장갑 슈트', ATTACK + f'\n[KW:Mobility]\n[STAT:AbilityRange] 내 코인 하나를 선택합니다.\n다음 보스 페이즈에 자신과 선택한 코인이 받는 피해를 각각 {W}만큼 감소시킵니다. 이 피해 감소는 각 코인에게 한 번씩 적용됩니다.\n이 효과를 받은 코인이 피해를 받으면, 다음 턴 동안 보스 패턴 공격력의 50%만큼 공격력이 증가합니다. 증가량은 반올림합니다.'),
    10: ('창의 적(방패)', ATTACK + f'\n[KW:Mobility]\n전방에 빈 칸이 있는 코인 하나를 선택합니다. 선택한 코인의 전방으로 이동합니다.\n[STAT:AbilityRange] 내에 있는 모든 코인이 받을 피해를 대신 받으며, 대신 받는 피해를 {W}만큼 감소시킵니다.'),
    11: ('과부화된 야바위 건틀릿', ATTACK + '\n[KW:Mobility]\n' + stat('[STAT:WeaponPower] * 1.2') + '%의 확률로 보스를 즉사시킵니다.\n보스 즉사에 실패하면 ' + stat('[STAT:WeaponPower] * 20') + f'%의 확률로 이 코인이 즉사합니다.\n코인 즉사 확률이 100%를 초과하면, [STAT:AbilityRange] 내 코인들에게 {A}만큼 피해를 준 뒤 이 코인이 즉사합니다.\n[KW:Hit]\n' + stat('23 - [STAT:AttackPower]') + '%의 확률로 보스에게 1부터 ' + stat('[STAT:AttackPower] * 5') + ' 사이의 무작위 추가 피해를 줍니다.'),
    12: ('응급처치키트', ATTACK.replace('[KW:Attack]', '[KW:Attack] [KW:Strike]') + f'\n[KW:Mobility] [KW:Continuous] : {C}\n{C}만큼 코인을 선택합니다.\n[STAT:AbilityRange] 내 코인을 선택하고, 선택한 코인이 ' + stat('[BUFF:Strike] + [STAT:WeaponPower]') + '만큼 체력을 회복합니다.'),
    13: ('십자형 보호막 전개기', ATTACK + f'\n[KW:Mobility]\n[STAT:AbilityRange] 내 모든 코인이 이번 턴 동안 {W}만큼의 보호막을 얻습니다.'),
    14: ('아드레날린 주입 권총', ATTACK.replace('[KW:Attack]', '[KW:Attack] [KW:Strike]') + f'\n[KW:Hit] [KW:Continuous] : {C}\n[STAT:AbilityRange] 내 코인을 선택하고, 선택한 코인은 이번 턴 동안 ' + stat('[STAT:WeaponPower] + [BUFF:Strike]') + '만큼 공격력이 증가합니다.'),
    15: ('(증폭) 조준 렌즈', ATTACK + f'\n[KW:Hit] [KW:Continuous] : {C}\n[STAT:AbilityRange] 내 코인을 선택하고, 선택한 코인의 [STAT:AttackRange]가 {W}만큼 증가합니다.'),
    16: ('긴급소생장치', ATTACK + '\n[KW:Mobility]\n[STAT:AbilityRange] 내 모든 코인에게 이번 턴 동안 긴급 소생 효과를 부여합니다.\n긴급 소생 효과를 가진 코인이 치명적인 피해를 받으면, 한 번에 한하여 체력 1로 생존합니다.'),
    17: ('이동식 분쇄 드릴', '[KW:Attack]\n[STAT:AttackRange] 내 적 하나에게 ' + stat('[STAT:AttackPower] + [BUFF:Absorb]') + f'만큼 피해를 줍니다.\n[KW:Mobility] [KW:Absorb] [KW:Continuous] : {C}\n선택한 장애물을 {W}만큼 [KW:Absorb]합니다.'),
    20: ('코르티솔 주입기', ATTACK.replace('[KW:Attack]', '[KW:Attack] [KW:Strike]') + f'\n[KW:Hit] [KW:Continuous] : {C}\n[STAT:AbilityRange] 내 코인을 선택하고, 선택한 코인은 이번 턴 동안 ' + stat('[STAT:WeaponPower] + [BUFF:Strike]') + '만큼 무기력이 증가합니다.'),
}

def snapshot(db):
    # 설명 컬럼만 제외한 모든 테이블의 행/스키마를 비교합니다.
    schema = db.execute('SELECT type,name,tbl_name,sql FROM sqlite_master ORDER BY type,name').fetchall()
    data = {}
    for (table,) in db.execute("SELECT name FROM sqlite_master WHERE type='table'"):
        quoted = '"' + table.replace('"', '""') + '"'
        columns = [r[1] for r in db.execute(f'PRAGMA table_info({quoted})')]
        if table == 'coin_weapon_def':
            columns.remove('KOR_DES')
        selection = ','.join('"' + c.replace('"', '""') + '"' for c in columns)
        data[table] = sorted(db.execute(f'SELECT {selection} FROM {quoted}').fetchall(), key=repr)
    return schema, data

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    path = ROOT / 'Content/DB.db'
    with sqlite3.connect(f'{path.as_uri()}?mode=ro', uri=True) as original:
        for identity, (name, description) in DESCRIPTIONS.items():
            assert original.execute('SELECT weapon_name FROM coin_weapon_def WHERE id=?', (identity,)).fetchone() == (name,)
            print(f'{identity} {name}\n{description}\n')
        if not args.apply:
            return
        folder = ROOT / 'Saved/Backups'
        folder.mkdir(parents=True, exist_ok=True)
        backup = folder / ('DB_before_stat_descriptions_' + datetime.datetime.now().strftime('%Y%m%d_%H%M%S_%f') + '.db')
        with sqlite3.connect(backup) as target:
            original.backup(target)
    with sqlite3.connect(path) as db, sqlite3.connect(backup) as old:
        db.execute('BEGIN IMMEDIATE')
        assert snapshot(db) == snapshot(old), 'DB changed since backup; refusing update'
        before = dict(db.execute('SELECT id,KOR_DES FROM coin_weapon_def'))
        assert before == dict(old.execute('SELECT id,KOR_DES FROM coin_weapon_def'))
        for identity, (_, description) in DESCRIPTIONS.items():
            db.execute('UPDATE coin_weapon_def SET KOR_DES=? WHERE id=?', (description.replace('\n', '\\n'), identity))
        assert snapshot(db) == snapshot(old), 'Unexpected non-description change'
        after = dict(db.execute('SELECT id,KOR_DES FROM coin_weapon_def'))
        assert all(after[k] == value for k, value in before.items() if k not in DESCRIPTIONS)
        assert db.execute('PRAGMA integrity_check').fetchone() == ('ok',)
        assert db.execute('PRAGMA foreign_key_check').fetchall() == old.execute('PRAGMA foreign_key_check').fetchall()
        db.commit()
        print(f'Updated {len(DESCRIPTIONS)} descriptions. All other data/schema unchanged. Backup: {backup}')

if __name__ == '__main__':
    main()
