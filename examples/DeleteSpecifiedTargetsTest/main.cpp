#include <vector>
#include <iostream>
#include <tr1/memory>

#include "dbcomm/DbComm.h"
#include "exception/ThrowableException.h"

using namespace std;
using namespace COMMON::DBCOMM;
using namespace COMMON::EXCEPTION;

int main()
{
	DbLocation dbLocation1;
	dbLocation1.SetDbId("TEST_DB1");
    dbLocation1.SetIp("127.0.0.1");
    dbLocation1.SetPort("3306");
    dbLocation1.SetUser("root");
    dbLocation1.SetPassword("123456");

	DbLocation dbLocation2;
    dbLocation2.SetDbId("TEST_DB2");
    dbLocation2.SetIp("127.0.0.1");
    dbLocation2.SetPort("3306");
    dbLocation2.SetUser("root");
    dbLocation2.SetPassword("123456");
    
	try
    {
        vector<DbLocation> dbLocations_array;
        dbLocations_array.push_back(dbLocation1);
        dbLocations_array.push_back(dbLocation2);

        tr1::shared_ptr<IDbTasks> mysqlTasks( new MysqlDbTasks(dbLocations_array, true) );
        mysqlTasks->Connect();

        bool success = false;
        DbExecuteAction* delete_action = mysqlTasks->Delete(5000);

        int i = 1;
        while (i <= 2)
        {
            stringstream ss;
            ss << "delete from tbl_test where id = " << i++;
            DeleteFilter deleteFilter(ss.str());
            delete_action->Do(&deleteFilter, &dbLocation1);
        }            

        delete_action->EndAction();
		
        DbDeleteRslt* delete_rslt = (DbDeleteRslt*)delete_action->GetRslt();
        map<DbLocation, long long> affected_rows = delete_rslt->GetAffectedRows(success);
        map<DbLocation, long long>::iterator it = affected_rows.begin();
        for (; it != affected_rows.end(); it++)
        {
            cout << "DELETE FROM " << it->first.ToString() << "[" << it->second << "]" << endl;
        }

        mysqlTasks->Disconnect();
    }
    catch (ThrowableException& e)
    {
        cout << e.What(true) << endl;
    }
    catch (...)
    {
        cout << "unknown exception" << std::endl;
    }
	
	return 0;
}