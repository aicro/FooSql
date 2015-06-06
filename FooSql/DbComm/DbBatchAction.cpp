#include "dbcomm/CommDef.h"
#include "dbcomm/DbTasks.h"
#include "dbcomm/DbBatchAction.h"
#include "dbcomm/BatchFilter.h"

namespace COMMON
{
    namespace DBCOMM
    {
        DbBatchAction::DbBatchAction(
            tr1::shared_ptr<IDbTasks> dbtasks, 
            tr1::shared_ptr<DbEngine> engine, 
            bool& is_action_finished, 
            int values_per_batch, 
            int times_to_commit)
            : DbInsertAction(dbtasks, engine, is_action_finished, times_to_commit)
        {
            values_per_batch_ = values_per_batch;
            
            vector<DbLocation>& dbs = dbtasks->GetDbLocations();
            for (int i = 0; i < dbs.size(); i++)
            {
                values_now_[dbs[i]] = 0;
            }
        }

        DbBatchAction::~DbBatchAction()
        {
        }

        bool DbBatchAction::MakeupStatement(const tr1::shared_ptr<StmtGenerator>& elem, BatchFilter* filter)
        {
            return elem->MakeupStatement(filter->GetColumns(), filter->GetTableName(), filter->GetValues(), filter->CheckCompatible());
        }

        bool DbBatchAction::Do(DbActionFilter* filter, map<DbLocation, long long>* affected_rows) throw (COMMON::EXCEPTION::ThrowableException)
        {
            return DbAction::Do(filter, affected_rows);
        }

        bool DbBatchAction::Do(DbActionFilter* f, DbLocation* location, long long* affected_rows) throw (COMMON::EXCEPTION::ThrowableException)
        {
            bool success = true;
            
            // clear the number of affected rows
            if (affected_rows)
            {
                *affected_rows = 0;
            }
            
            // check whether the input statement is compatible with 
			// the current one, if not do the current one at once
            unsigned int& value_count = values_now_[*location]; 
            tr1::shared_ptr<StmtGenerator>& elem = elems_[*location];
            
            BatchFilter* filter = (BatchFilter*)f;
            bool do_right_now = MakeupStatement(elem, filter);
            if (do_right_now == true)
            {
                // change the count to meet the limite
                value_count = values_per_batch_;
            }
            else
            {
                value_count++;
            }

            // do we meet the limit?
            if (value_count == values_per_batch_)
            {
                value_count = 0;  
                
                string statement = elem->FormStatement(*location);
                
                elem->ClearContent();
                
                if (statement != "")
                {
                    InsertFilter filter(statement);
                    success = DbInsertAction::Do(&filter, location, affected_rows);
                }
            }
 
            // set the new statement
            if (do_right_now)
            {
                MakeupStatement(elem, (BatchFilter*)f);
                value_count++;    
            }

            return success;
        }
        
        bool DbBatchAction::Do(
            map<DbLocation, DbActionFilter*>& works, map<DbLocation, long long>* affected_rows) throw (COMMON::EXCEPTION::ThrowableException)
        {
            bool success = true;
            
            // clear the last result
            if (affected_rows)
            {
                map<DbLocation, long long>::iterator it_clear = affected_rows->begin();
                while (it_clear != affected_rows->end())
                {
                    it_clear->second = 0;
                    it_clear++;
                }
            }
            
			// we will check the new input statements for compatibility with the exsiting commands.
			// if they are of different types, that is with different table to insert or different 
			// specified columns, we should do the exsiting commands right now. 
            map<DbLocation, tr1::shared_ptr<StmtGenerator> > remains;
            map<DbLocation, DbActionFilter*>::iterator it = works.begin();
            for (; it != works.end(); it++)
            {
                BatchFilter* filter = (BatchFilter*)it->second;
                bool do_right_now = MakeupStatement(elems_[it->first], filter);
                if (do_right_now == true)
                {
					// the new input statement is not compatible withe the existing ones.
					
					// finish the old ones by specify the current count of values 
                    values_now_[it->first] = values_per_batch_;
                    
					// save the current input statements for later use
                    remains[it->first] = elems_[it->first];
                }
                else
                {
					// the new input statement is compatible with the old one, 
					// only add count.
                    values_now_[it->first]++;
                }
            }

            // now to check each db locations. If they have buffered enough values
			// just finish them.
			map<DbLocation, DbActionFilter*> real_works;
            
            // prepare the vector before using them
            vector<DbActionFilter> real_filters(values_now_.size());
            map<DbLocation, unsigned int>::iterator vn_it = values_now_.begin();
            for (int i = 0 ; vn_it != values_now_.end(); vn_it++, i++)
            {
                if (vn_it->second >= values_per_batch_)
                {
					// limits have been met, do!
					
                    // clear the count
                    vn_it->second = 0;  
                
                    // generate SQL statements
                    string statement = elems_[vn_it->first]->FormStatement(vn_it->first);
                    
                    // erase the original contents
                    elems_[vn_it->first]->ClearContent();
                    
                    if (statement == "")
                    {
						// nothing to be done? skip it
                        continue;
                    }
                    
                    real_filters[i].SetContents(statement);
                    real_works[vn_it->first] = &real_filters[i];
                }                
            }
           
            // work?
            if (real_works.size() > 0)
            {
                success = DbInsertAction::Do(real_works, affected_rows);
            }
            
			// if the newly input commands are incompatible with the existing ones,
			// we should do the exsiting first, which have been done previously.
			// And now, we should rebuild the new input commands to make it cached.
            map<DbLocation, tr1::shared_ptr<StmtGenerator> >::iterator remains_it = remains.begin();
            for ( ; remains_it != remains.end(); remains_it++)
            {
                // cause we have done the exsiting commands, no conflicts should happen
                MakeupStatement(elems_[remains_it->first], (BatchFilter*)(works[remains_it->first]));
                values_now_[remains_it->first]++;
            }

            return success;
        }

        bool DbBatchAction::EndAction(map<DbLocation, long long>* affected_rows) throw (COMMON::EXCEPTION::ThrowableException)
        {
			// do all the cached but not done commands
            map<DbLocation, unsigned int>::iterator it = values_now_.begin();
            for (; it != values_now_.end(); it++)
            {
                it->second = values_per_batch_;
            }

            return DoAllLeft(affected_rows) && DbInsertAction::EndAction();
        }

        bool DbBatchAction::DoAllLeft(map<DbLocation, long long>* affected_rows)
        {
            bool success = false;

            map<DbLocation, DbActionFilter*> works;
            vector<BatchFilter> filters(elems_.size());
            
            map<DbLocation, tr1::shared_ptr<StmtGenerator> >::iterator it = elems_.begin();
            for ( int i = 0; it != elems_.end(); it++, i++)
            {
				// generate an empty statement, adjust the counts and execute now
                works[it->first] = &(filters[i]);
                values_now_[it->first] = values_per_batch_;
            }

            return Do(works, affected_rows);
        }        
    }
}
