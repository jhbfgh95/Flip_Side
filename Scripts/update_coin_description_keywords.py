"""Approved CoinSlot description migration; dry-run by default, SQLite backup before writes."""
import argparse
import datetime
import json
import sqlite3
import sys
import uuid
from pathlib import Path


def snapshot(connection):
    tables = [row[0] for row in connection.execute(
        "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%' ORDER BY name")]
    return {table: connection.execute('SELECT * FROM "' + table.replace('"', '""') + '" ORDER BY rowid').fetchall()
            for table in tables}


def main():
    sys.stdout.reconfigure(encoding="utf-8")
    parser = argparse.ArgumentParser()
    parser.add_argument("--apply", action="store_true")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    db_path = root / "Content" / "DB.db"
    connection = sqlite3.connect(db_path.as_uri() + ("?mode=rw" if args.apply else "?mode=ro"), uri=True)
    connection.execute("PRAGMA foreign_keys=ON")
    try:
        burger = connection.execute("SELECT KOR_DES FROM coin_weapon_def WHERE id=3").fetchone()[0]
        drill = connection.execute("SELECT KOR_DES FROM coin_weapon_def WHERE id=17").fetchone()[0]
        newline = "\\n" if "\\n" in burger else "\n"
        miss_marker = newline + "[KW:Miss]"
        if miss_marker in burger:
            before, tail = burger.split(miss_marker, 1)
            following_header = tail.find(newline + "[KW:")
            new_burger = before + (tail[following_header:] if following_header >= 0 else "")
        else:
            new_burger = burger
        new_drill = drill.replace("[KW:Mobility] [KW:Continuous]", "[KW:Mobility] [KW:Absorb] [KW:Continuous]", 1)
        assert "[KW:Miss]" not in new_burger
        assert "[KW:Mobility] [KW:Absorb] [KW:Continuous]" in new_drill
        other_miss = connection.execute("SELECT id FROM coin_weapon_def WHERE id<>3 AND (KOR_DES LIKE '%[KW:Miss]%' OR ENG_DES LIKE '%[KW:Miss]%')").fetchall()
        assert not other_miss, f"Unexpected other Miss references: {other_miss}"
        order = ["Attack", "Mobility", "Hit", "Continuous", "Absorb", "Strike"]
        assert all(connection.execute("SELECT 1 FROM keyword_definition WHERE keyword_code=?", (code,)).fetchone() for code in order)
        print(json.dumps({"burger_after": new_burger, "drill_after": new_drill, "keyword_order": order}, ensure_ascii=False, indent=2))
        if not args.apply:
            return

        backup_dir = root / "Saved" / "Backups" / "CoinDescription"
        backup_dir.mkdir(parents=True, exist_ok=True)
        stamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        backup_path = backup_dir / f"DB_{stamp}_{uuid.uuid4().hex[:8]}.db"
        with sqlite3.connect(backup_path) as backup:
            connection.backup(backup)
        print("BACKUP:", backup_path)
        connection.execute("BEGIN IMMEDIATE")
        original = snapshot(connection)
        original_schema = connection.execute("SELECT type,name,sql FROM sqlite_master ORDER BY type,name").fetchall()
        # A concurrent edit after planning must not be overwritten.
        assert connection.execute("SELECT KOR_DES FROM coin_weapon_def WHERE id=3").fetchone()[0] == burger
        assert connection.execute("SELECT KOR_DES FROM coin_weapon_def WHERE id=17").fetchone()[0] == drill
        connection.execute("UPDATE coin_weapon_def SET KOR_DES=? WHERE id=3", (new_burger,))
        connection.execute("UPDATE coin_weapon_def SET KOR_DES=? WHERE id=17", (new_drill,))
        connection.execute("DELETE FROM keyword_definition WHERE keyword_code='Miss'")
        for index, code in enumerate(order, 1):
            connection.execute("UPDATE keyword_definition SET sort_order=? WHERE keyword_code=?", (index, code))
        updated = snapshot(connection)
        for table in original:
            if table not in ("coin_weapon_def", "keyword_definition"):
                assert original[table] == updated[table], f"Unrelated table changed: {table}"
        columns = [row[1] for row in connection.execute("PRAGMA table_info(coin_weapon_def)")]
        description_column = columns.index("KOR_DES")
        id_column = columns.index("id")
        expected_weapons = []
        for original_row in original["coin_weapon_def"]:
            row = list(original_row)
            if row[id_column] == 3: row[description_column] = new_burger
            if row[id_column] == 17: row[description_column] = new_drill
            expected_weapons.append(tuple(row))
        assert updated["coin_weapon_def"] == expected_weapons
        keyword_columns = [row[1] for row in connection.execute("PRAGMA table_info(keyword_definition)")]
        code_column = keyword_columns.index("keyword_code")
        sort_column = keyword_columns.index("sort_order")
        expected_keywords = []
        for original_row in original["keyword_definition"]:
            row = list(original_row)
            if row[code_column] == "Miss": continue
            if row[code_column] in order: row[sort_column] = order.index(row[code_column]) + 1
            expected_keywords.append(tuple(row))
        assert updated["keyword_definition"] == expected_keywords
        assert original_schema == connection.execute("SELECT type,name,sql FROM sqlite_master ORDER BY type,name").fetchall()
        assert connection.execute("PRAGMA integrity_check").fetchone()[0] == "ok"
        assert not connection.execute("PRAGMA foreign_key_check").fetchall()
        connection.commit()
        print("VERIFIED: only approved description/keyword changes; integrity and foreign keys OK.")
    except Exception:
        connection.rollback()
        raise
    finally:
        connection.close()


if __name__ == "__main__":
    main()
