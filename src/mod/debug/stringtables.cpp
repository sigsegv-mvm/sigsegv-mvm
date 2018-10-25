#include "mod.h"


namespace Mod::Debug::StringTables
{
	void WriteTableToFile(INetworkStringTable *table)
	{
		CFmtStrN<256> path("tf/stringtables/%s", table->GetTableName());
		
		FILE *f = fopen(path, "w+");
		if (f == nullptr) {
			Warning("Couldn't open file \"%s\" for writing\n", path.Get());
			return;
		}
		
		for (int i = 0; i < table->GetNumStrings(); ++i) {
			fprintf(f, "%5d|%5u|%s\n", i, strlen(table->GetString(i)), table->GetString(i));
		}
		
		fclose(f);
	}
	
	
	CON_COMMAND(sig_debug_stringtables, "Debug: dump string table information")
	{
		size_t total = 0;
		
		DevMsg("%-20s  %11s  %6s\n",
			"TABLE", "ENTRIES", "BYTES");
		
		for (int i = 0; i < networkstringtable->GetNumTables(); ++i) {
			INetworkStringTable *table = networkstringtable->GetTable(i);
			if (table == nullptr) {
				Warning("INetworkStringTable::GetTable(%d) returned nullptr\n", i);
				continue;
			}
			
			WriteTableToFile(table);
			
			CUtlBuffer buf;
			buf.PutInt(table->GetNumStrings());
			for (int j = 0; j < table->GetNumStrings(); ++j) {
				buf.PutString(table->GetString(j));
				
				int user_data_size;
				const void *user_data = table->GetStringUserData(j, &user_data_size);
				
				if (user_data_size > 0) {
					buf.PutChar(1);
					buf.PutShort(user_data_size);
					buf.Put(user_data, user_data_size);
				} else {
					buf.PutChar(0);
				}
			}
			
			DevMsg("%-20s  %5d/%5d  %6d\n",
				table->GetTableName(), table->GetNumStrings(), table->GetMaxStrings(),
				buf.TellPut());
			total += buf.TellPut();
		}
		
		DevMsg("Total size of all string tables: %d bytes\n", total);
	}
}
