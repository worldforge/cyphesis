//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_Database_selectEntities
#define STUB_Database_selectEntities
DatabaseResult Database::selectEntities(const std::string & loc)
{
    return DatabaseResult(nullptr);
}
#endif //STUB_Database_selectEntities

#ifndef STUB_Database_selectRelation
#define STUB_Database_selectRelation
DatabaseResult Database::selectRelation(const std::string & name, const std::string & id)
{
    return DatabaseResult(nullptr);
}
#endif //STUB_Database_selectRelation

#ifndef STUB_Database_selectSimpleRowBy
#define STUB_Database_selectSimpleRowBy
DatabaseResult Database::selectSimpleRowBy(const std::string & name, const std::string & column, const std::string & value)
{
    return DatabaseResult(nullptr);
}
#endif //STUB_Database_selectSimpleRowBy


#ifndef STUB_Database_selectProperties
#define STUB_Database_selectProperties
DatabaseResult Database::selectProperties(const std::string & loc)
{
    return DatabaseResult(nullptr);
}
#endif //STUB_Database_selectProperties

#ifndef STUB_Database_selectThoughts
#define STUB_Database_selectThoughts
DatabaseResult Database::selectThoughts(const std::string & loc)
{
    return DatabaseResult(nullptr);
}
#endif //STUB_Database_selectThoughts

#ifndef STUB_Database_runSimpleSelectQuery
#define STUB_Database_runSimpleSelectQuery
DatabaseResult Database::runSimpleSelectQuery(const std::string& query)
{
    return DatabaseResult(nullptr);
}
#endif //STUB_Database_runSimpleSelectQuery

#ifndef STUB_DatabaseResult_DatabaseResult
#define STUB_DatabaseResult_DatabaseResult
DatabaseResult::DatabaseResult(DatabaseResult&& dr) noexcept
{

}
#endif //STUB_DatabaseResult_DatabaseResult

DatabaseResult::const_iterator& DatabaseResult::const_iterator::operator++()
{
    return *this;
}

DatabaseResult::const_iterator::const_iterator(std::unique_ptr<DatabaseResult::const_iterator_worker>&& worker, const DatabaseResult::DatabaseResultWorker& dr)
    : m_worker(std::move(worker)),
      m_dr(dr)
{
}
