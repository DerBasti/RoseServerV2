#ifndef __DATABASE_QUERIES__
#define __DATABASE_QUERIES__

#ifdef _MSC_VER
#pragma once
#endif

class DBQueries {
private:
	DBQueries() {}
	~DBQueries() {}
public:
	class Select {
		public:
			static const char *ACCOUNT_INFOS;
			static const char *ACCOUNT_NAME_BY_ID;
			static const char *ALL_CHARACTERS_FROM_ACCOUNT;
			static const char *CHARSELECT_EQUIPMENT;
			static const char *CHARACTER_ID_FROM_ACC;
			static const char *LAST_CHAR_AND_ACCESSLEVEL;
			static const char *CHARACTER_BASIC_INFORMATION;
			static const char *CHARACTER_STATS;
			static const char *CHARACTER_INVENTORY;
	};
	class Insert {
		public:
			static const char* ADD_ITEM_MINIMALISTIC;
			static const char* ADD_DEFAULT_SKILLS;
			static const char* ADD_DEFAULT_QUESTS; 
			static const char *ADD_DEFAULT_STATS;
			static const char *ADD_CHARACTER;
	};
	class Update {
		public:
			static const char* LAST_PLAYED_CHAR;
	};
};

#endif //__DATABASE_QUERIES__