#include "DatabaseQueries.h"

const char *DBQueries::Select::ACCOUNT_INFOS = "SELECT id, password, accesslevel, isonline FROM accounts WHERE name=?";
const char *DBQueries::Select::ACCOUNT_NAME_BY_ID = "SELECT name from accounts WHERE id=?";
const char *DBQueries::Select::ALL_CHARACTERS_FROM_ACCOUNT = "SELECT * from characters WHERE acc_id=?";
const char *DBQueries::Select::CHARSELECT_EQUIPMENT = "SELECT slot, itemId, refine from inventory WHERE charId=? AND slot < 9";
const char *DBQueries::Select::CHARACTER_ID_FROM_ACC = "SELECT id FROM characters WHERE acc_id=? AND name=?";
const char *DBQueries::Select::LAST_CHAR_AND_ACCESSLEVEL = "SELECT lastChar_Id, accesslevel FROM accounts WHERE id=?";
const char *DBQueries::Select::CHARACTER_BASIC_INFORMATION = "SELECT name, level, experience, job, face, hair, sex, zulies, saveTown, respawnMap, respawnId FROM characters WHERE id=?";
const char *DBQueries::Select::CHARACTER_STATS = "SELECT * FROM character_stats WHERE id=?";
const char *DBQueries::Select::CHARACTER_INVENTORY = "SELECT * FROM inventory WHERE charId=?";

/*
================================================================
*/
const char *DBQueries::Insert::ADD_ITEM_MINIMALISTIC = "INSERT INTO inventory(charId, slot, itemId) VALUES(?, ?, ?)";
const char *DBQueries::Insert::ADD_DEFAULT_SKILLS = "INSERT INTO character_skills(id, basicSkills, passiveSkills, activeSkills) VALUES(?, \
															'11, 12, 16, 20, 41, 42, 43, 181, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0', \
																	'0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0', \
																					'0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0')";
const char *DBQueries::Insert::ADD_DEFAULT_QUESTS = "INSERT INTO character_quest_flags(charId, questFlags, episodeFlags, jobFlags, planetFlags, fractionFlags) \
															VALUES(?, '0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0', \
																'0,0,0,0,0', '0,0,0', '0,0,0,0,0,0,0', '0,0,0,0,0,0,0,0,0,0')";
const char *DBQueries::Insert::ADD_DEFAULT_STATS = "INSERT INTO character_stats(id) VALUES(?)";

const char *DBQueries::Insert::ADD_CHARACTER = "INSERT INTO characters (acc_id, name, level, job, face, hair, sex) VALUES(?, ?, ?, ?, ?, ?, ?)";

/*
===============================================================
*/
const char*DBQueries::Update::LAST_PLAYED_CHAR = "UPDATE accounts SET lastChar_Id=? WHERE id=?";