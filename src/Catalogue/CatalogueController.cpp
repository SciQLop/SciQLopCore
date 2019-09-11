#include <Catalogue/CatalogueController.h>
#include <CatalogueIO.hpp>
#include <containers/algorithms.hpp>
#include <Common/debug.h>
#include <QDataStream>
#include <QDir>
#include <QMutex>
#include <QStandardPaths>
#include <QThread>

using namespace cpp_utils::containers;

// class CatalogueController::CatalogueControllerPrivate
//{

// public:
//    explicit CatalogueControllerPrivate(CatalogueController* parent) : m_Q {
//    parent } {}

//    CatalogueDao m_CatalogueDao;

//    QStringList m_RepositoryList;
//    CatalogueController* m_Q;

//    QSet<QString> m_KeysWithChanges;

//    QString eventUniqueKey(const std::shared_ptr<DBEvent>& event) const;
//    QString catalogueUniqueKey(const std::shared_ptr<DBCatalogue>& catalogue)
//    const;

//    void copyDBtoDB(const QString& dbFrom, const QString& dbTo);
//    QString toWorkRepository(QString repository);
//    QString toSyncRepository(QString repository);
//    void savAllDB();

//    void saveEvent(std::shared_ptr<DBEvent> event, bool persist = true);
//    void saveCatalogue(std::shared_ptr<DBCatalogue> catalogue, bool persist =
//    true);

//    std::shared_ptr<IRequestPredicate> createFinder(
//        const QUuid& uniqId, const QString& repository, DBType type);
//};

CatalogueController::CatalogueController(QObject* parent)
//: impl { spimpl::make_unique_impl<CatalogueControllerPrivate>(this) }
{}

CatalogueController::~CatalogueController() {}

QStringList CatalogueController::repositories() const
{
  QStringList repos;
  std::transform(std::begin(_currentRepos), std::end(_currentRepos),
                 std::back_inserter(repos),
                 [](auto& pair) { return pair.first; });
  return repos;
}

void CatalogueController::loadRepository(const QString& path,
                                         const QString& name)
{
  if(QFile::exists(path))
  {
    auto repo = CatalogiCpp::load_repository<time_t>(path.toStdString());
    _lastSavedRepos[name] = repo;
    _currentRepos[name]   = repo;
  }
}

void CatalogueController::saveRepository(const QString& path,
                                         const QString& name)
{
  if(contains(_currentRepos, name))
  {
    CatalogiCpp::save_repository(_currentRepos[name], path.toStdString());
    _lastSavedRepos[name] = _currentRepos[name];
  }
}

std::vector<CatalogueController::Event_ptr> CatalogueController::events()
{
  std::vector<CatalogueController::Event_ptr> e_list;
  for(auto& [_, repo] : _currentRepos)
  {
    for(auto& [_, event] : repo.events())
      e_list.push_back(event);
  }
  return e_list;
}

std::vector<CatalogueController::Event_ptr>
CatalogueController::events(const CatalogueController::Catalogue_ptr& catalogue)
{
  std::vector<CatalogueController::Event_ptr> e_list;
  for(auto& [_, event] : catalogue->events())
    e_list.push_back(event);
  return e_list;
}

std::vector<std::shared_ptr<CatalogueController::Event_t>>
CatalogueController::events(const QString& repository)
{
  std::vector<std::shared_ptr<CatalogueController::Event_t>> e_list;
  if(contains(_currentRepos, repository))
  {
    auto repo = _currentRepos[repository];
    for(auto& [_, event] : repo.events())
      e_list.push_back(event);
  }
  return e_list;
}

std::vector<CatalogueController::Catalogue_ptr>
CatalogueController::catalogues()
{
  std::vector<CatalogueController::Catalogue_ptr> c_list;
  for(auto& [_, repo] : _currentRepos)
  {
    for(auto& [_, catalogue] : repo.catalogues())
      c_list.push_back(catalogue);
  }
  return c_list;
}

std::vector<CatalogueController::Catalogue_ptr>
CatalogueController::catalogues(const QString& repository)
{
  std::vector<CatalogueController::Catalogue_ptr> c_list;
  if(contains(_currentRepos, repository))
  {
    auto repo = _currentRepos[repository];
    for(auto& [_, catalogue] : repo.catalogues())
      c_list.push_back(catalogue);
  }
  return c_list;
}

bool CatalogueController::hasUnsavedChanges(
    CatalogueController::Event_ptr event)
{
  if(auto repo = repository(event))
  {
    if(contains(_lastSavedRepos, *repo))
    { return *event != *(_lastSavedRepos[*repo].event(event->uuid)); }
  }
  return true;
}

std::optional<QString>
CatalogueController::repository(CatalogueController::Event_ptr event)
{
  for(auto& [repoName, repo] : _currentRepos)
  {
    if(repo.event(event->uuid)) return repoName;
  }
  return std::nullopt;
}

std::optional<QString>
CatalogueController::repository(CatalogueController::Catalogue_ptr catalogue)
{
  for(auto& [repoName, repo] : _currentRepos)
  {
    if(repo.catalogue(catalogue->uuid)) return repoName;
  }
  return std::nullopt;
}

void CatalogueController::save(CatalogueController::Event_ptr event)
{
  auto repo_name = repository(event);
  if(repo_name && contains(_lastSavedRepos, *repo_name))
  {
    auto repo = _lastSavedRepos[*repo_name];
    if(contains(repo.events(), event->uuid))
    {
      auto saved_evemt = _lastSavedRepos[*repo_name].event(event->uuid);
      *saved_evemt     = *event;
    }
    else
    {
      // TODO/Question should we also take care of which catalogue has it?
      // if an event is created and added to a catalogue, what should we do if
      // the user clicks the event save button?
      //  - Only save event value in all events list?
      //  - Also save the fact that this event is added to a catalogue and save
      //    the catalogue?
      repo.add(event);
    }
  }
}

void CatalogueController::save(CatalogueController::Catalogue_ptr catalogue)
{
  auto repo_name = repository(catalogue);
  if(repo_name && contains(_lastSavedRepos, *repo_name))
  {
    auto repo = _lastSavedRepos[*repo_name];
    if(contains(repo.catalogues(), catalogue->uuid))
    {
      auto saved_catalogue = repo.catalogue(catalogue->uuid);
      *saved_catalogue     = *catalogue;
    }
    else
    {
      repo.add(catalogue);
    }
  }
}

void CatalogueController::save(const QString& repository)
{
  if(contains(_currentRepos, repository))
  { _lastSavedRepos[repository] = _currentRepos[repository]; }
  else
  {
    SCIQLOP_ERROR(CatalogueController, "Trying to save an unknown repository");
  }
}

void CatalogueController::add(const QString& repository)
{
  if(!contains(_currentRepos, repository))
  {
    _currentRepos[repository] = Repository_t{};
    emit repositoryAdded(repository);
  }
}

CatalogueController::Catalogue_ptr
CatalogueController::add(const QString& catalogue, const QString& repository)
{
  if(!contains(_currentRepos, repository)) { add(repository); }
  auto new_catalogue  = make_catalogue_ptr();
  new_catalogue->name = catalogue.toStdString();
  _currentRepos[repository].add(new_catalogue);
  emit catalogueAdded(new_catalogue, repository);
  return new_catalogue;
}

void CatalogueController::add(CatalogueController::Event_ptr event,
                              CatalogueController::Catalogue_ptr catalogue)
{
  catalogue->add(event);
  emit this->catalogueChanged(catalogue);
}

void CatalogueController::add(CatalogueController::Event_ptr event,
                              const QString& repository)
{}

// void CatalogueController::saveDB(const QString& destinationPath, const
// QString& repositoryName)
//{
//    if (!impl->m_CatalogueDao.saveDB(destinationPath, repositoryName))
//    {
//        qCCritical(LOG_CatalogueController())
//            << tr("Impossible to saveDB %1 from %2 ").arg(repositoryName,
//            destinationPath);
//    }
//}

// std::list<std::shared_ptr<DBEvent>> CatalogueController::retrieveEvents(
//    const QString& repository) const
//{
//    QString dbDireName = repository.isEmpty() ? REPOSITORY_DEFAULT :
//    repository;

//    auto eventsShared = std::list<std::shared_ptr<DBEvent>> {};
//    auto events =
//    impl->m_CatalogueDao.getEvents(impl->toWorkRepository(dbDireName)); for
//    (auto event : events)
//    {
//        eventsShared.push_back(std::make_shared<DBEvent>(event));
//    }
//    return eventsShared;
//}

// std::list<std::shared_ptr<DBEvent>> CatalogueController::retrieveAllEvents()
// const
//{
//    auto eventsShared = std::list<std::shared_ptr<DBEvent>> {};
//    for (auto repository : impl->m_RepositoryList)
//    {
//        eventsShared.splice(eventsShared.end(), retrieveEvents(repository));
//    }

//    return eventsShared;
//}

// std::list<std::shared_ptr<DBEvent>>
// CatalogueController::retrieveEventsFromCatalogue(
//    std::shared_ptr<DBCatalogue> catalogue) const
//{
//    auto eventsShared = std::list<std::shared_ptr<DBEvent>> {};
//    auto events = impl->m_CatalogueDao.getCatalogueEvents(*catalogue);
//    for (auto event : events)
//    {
//        eventsShared.push_back(std::make_shared<DBEvent>(event));
//    }
//    return eventsShared;
//}

// void CatalogueController::updateEvent(std::shared_ptr<DBEvent> event)
//{
//    event->setRepository(impl->toWorkRepository(event->getRepository()));

//    auto uniqueId = impl->eventUniqueKey(event);
//    impl->m_KeysWithChanges.insert(uniqueId);

//    impl->m_CatalogueDao.updateEvent(*event);
//}

// void CatalogueController::updateEventProduct(std::shared_ptr<DBEventProduct>
// eventProduct)
//{
//    impl->m_CatalogueDao.updateEventProduct(*eventProduct);
//}

// void CatalogueController::removeEvent(std::shared_ptr<DBEvent> event)
//{
//    // Remove it from both repository and repository_work
//    event->setRepository(impl->toWorkRepository(event->getRepository()));
//    impl->m_CatalogueDao.removeEvent(*event);
//    event->setRepository(impl->toSyncRepository(event->getRepository()));
//    impl->m_CatalogueDao.removeEvent(*event);
//    impl->savAllDB();
//}

// void CatalogueController::addEvent(std::shared_ptr<DBEvent> event)
//{
//    event->setRepository(impl->toWorkRepository(event->getRepository()));

//    auto eventTemp = *event;
//    impl->m_CatalogueDao.addEvent(eventTemp);

//    // Call update is necessary at the creation of add Event if it has some
//    tags or some event
//    // products
//    if (!event->getEventProducts().empty() || !event->getTags().empty())
//    {

//        auto eventProductsTemp = eventTemp.getEventProducts();
//        auto eventProductTempUpdated = std::list<DBEventProduct> {};
//        for (auto eventProductTemp : eventProductsTemp)
//        {
//            eventProductTemp.setEvent(eventTemp);
//            eventProductTempUpdated.push_back(eventProductTemp);
//        }
//        eventTemp.setEventProducts(eventProductTempUpdated);

//        impl->m_CatalogueDao.updateEvent(eventTemp);
//    }

//    auto workPred = impl->createFinder(event->getUniqId(),
//    event->getRepository(), DBType::WORK);

//    auto workEvent = impl->m_CatalogueDao.getEvent(workPred);
//    *event = workEvent;

//    auto uniqueId = impl->eventUniqueKey(event);
//    impl->m_KeysWithChanges.insert(uniqueId);
//}

// void CatalogueController::saveEvent(std::shared_ptr<DBEvent> event)
//{
//    impl->saveEvent(event, true);
//    impl->m_KeysWithChanges.remove(impl->eventUniqueKey(event));
//}

// void CatalogueController::discardEvent(std::shared_ptr<DBEvent> event, bool&
// removed)
//{
//    auto syncPred = impl->createFinder(event->getUniqId(),
//    event->getRepository(), DBType::SYNC); auto workPred =
//    impl->createFinder(event->getUniqId(), event->getRepository(),
//    DBType::WORK);

//    auto syncEvent = impl->m_CatalogueDao.getEvent(syncPred);
//    if (!syncEvent.getUniqId().isNull())
//    {
//        removed = false;
//        impl->m_CatalogueDao.copyEvent(
//            syncEvent, impl->toWorkRepository(event->getRepository()), true);

//        auto workEvent = impl->m_CatalogueDao.getEvent(workPred);
//        *event = workEvent;
//        impl->m_KeysWithChanges.remove(impl->eventUniqueKey(event));
//    }
//    else
//    {
//        removed = true;
//        // Since the element wasn't in sync repository. Discard it means
//        remove it
//        event->setRepository(impl->toWorkRepository(event->getRepository()));
//        impl->m_CatalogueDao.removeEvent(*event);
//    }
//}

// bool CatalogueController::eventHasChanges(std::shared_ptr<DBEvent> event)
// const
//{
//    return impl->m_KeysWithChanges.contains(impl->eventUniqueKey(event));
//}

// std::list<std::shared_ptr<DBCatalogue>>
// CatalogueController::retrieveCatalogues(
//    const QString& repository) const
//{
//    QString dbDireName = repository.isEmpty() ? REPOSITORY_DEFAULT :
//    repository;

//    auto cataloguesShared = std::list<std::shared_ptr<DBCatalogue>> {};
//    auto catalogues =
//    impl->m_CatalogueDao.getCatalogues(impl->toWorkRepository(dbDireName));
//    for (auto catalogue : catalogues)
//    {
//        cataloguesShared.push_back(std::make_shared<DBCatalogue>(catalogue));
//    }
//    return cataloguesShared;
//}

// void CatalogueController::addCatalogue(std::shared_ptr<DBCatalogue>
// catalogue)
//{
//    catalogue->setRepository(impl->toWorkRepository(catalogue->getRepository()));

//    auto catalogueTemp = *catalogue;
//    impl->m_CatalogueDao.addCatalogue(catalogueTemp);

//    auto workPred
//        = impl->createFinder(catalogue->getUniqId(),
//        catalogue->getRepository(), DBType::WORK);

//    auto workCatalogue = impl->m_CatalogueDao.getCatalogue(workPred);
//    *catalogue = workCatalogue;

//    auto uniqueId = impl->catalogueUniqueKey(catalogue);
//    impl->m_KeysWithChanges.insert(uniqueId);
//}

// void CatalogueController::updateCatalogue(std::shared_ptr<DBCatalogue>
// catalogue)
//{
//    catalogue->setRepository(impl->toWorkRepository(catalogue->getRepository()));

//    auto uniqueId = impl->catalogueUniqueKey(catalogue);
//    impl->m_KeysWithChanges.insert(uniqueId);

//    impl->m_CatalogueDao.updateCatalogue(*catalogue);
//}

// void CatalogueController::removeCatalogue(std::shared_ptr<DBCatalogue>
// catalogue)
//{
//    // Remove it from both repository and repository_work
//    catalogue->setRepository(impl->toWorkRepository(catalogue->getRepository()));
//    impl->m_CatalogueDao.removeCatalogue(*catalogue);
//    catalogue->setRepository(impl->toSyncRepository(catalogue->getRepository()));
//    impl->m_CatalogueDao.removeCatalogue(*catalogue);
//    impl->savAllDB();
//}

// void CatalogueController::saveCatalogue(std::shared_ptr<DBCatalogue>
// catalogue)
//{
//    impl->saveCatalogue(catalogue, true);
//    impl->m_KeysWithChanges.remove(impl->catalogueUniqueKey(catalogue));

//    // remove key of events of the catalogue
//    if (catalogue->getType() == CatalogueType::STATIC)
//    {
//        auto events = this->retrieveEventsFromCatalogue(catalogue);
//        for (auto event : events)
//        {
//            impl->m_KeysWithChanges.remove(impl->eventUniqueKey(event));
//        }
//    }
//}

// void CatalogueController::discardCatalogue(std::shared_ptr<DBCatalogue>
// catalogue, bool& removed)
//{
//    auto syncPred
//        = impl->createFinder(catalogue->getUniqId(),
//        catalogue->getRepository(), DBType::SYNC);
//    auto workPred
//        = impl->createFinder(catalogue->getUniqId(),
//        catalogue->getRepository(), DBType::WORK);

//    auto syncCatalogue = impl->m_CatalogueDao.getCatalogue(syncPred);
//    if (!syncCatalogue.getUniqId().isNull())
//    {
//        removed = false;
//        impl->m_CatalogueDao.copyCatalogue(
//            syncCatalogue, impl->toWorkRepository(catalogue->getRepository()),
//            true);

//        auto workCatalogue = impl->m_CatalogueDao.getCatalogue(workPred);
//        *catalogue = workCatalogue;
//        impl->m_KeysWithChanges.remove(impl->catalogueUniqueKey(catalogue));
//    }
//    else
//    {
//        removed = true;
//        // Since the element wasn't in sync repository. Discard it means
//        remove it
//        catalogue->setRepository(impl->toWorkRepository(catalogue->getRepository()));
//        impl->m_CatalogueDao.removeCatalogue(*catalogue);
//    }
//}

// void CatalogueController::saveAll()
//{
//    for (auto repository : impl->m_RepositoryList)
//    {
//        // Save Event
//        auto events = this->retrieveEvents(repository);
//        for (auto event : events)
//        {
//            impl->saveEvent(event, false);
//        }

//        // Save Catalogue
//        auto catalogues = this->retrieveCatalogues(repository);
//        for (auto catalogue : catalogues)
//        {
//            impl->saveCatalogue(catalogue, false);
//        }
//    }

//    impl->savAllDB();
//    impl->m_KeysWithChanges.clear();
//}

// bool CatalogueController::hasChanges() const
//{
//    return !impl->m_KeysWithChanges.isEmpty();
//}

// QByteArray CatalogueController::mimeDataForEvents(
//    const QVector<std::shared_ptr<DBEvent>>& events) const
//{
//    auto encodedData = QByteArray {};

//    QMap<QString, QVariantList> idsPerRepository;
//    for (auto event : events)
//    {
//        idsPerRepository[event->getRepository()] << event->getUniqId();
//    }

//    QDataStream stream { &encodedData, QIODevice::WriteOnly };
//    stream << idsPerRepository;

//    return encodedData;
//}

// QVector<std::shared_ptr<DBEvent>> CatalogueController::eventsForMimeData(
//    const QByteArray& mimeData) const
//{
//    auto events = QVector<std::shared_ptr<DBEvent>> {};
//    QDataStream stream { mimeData };

//    QMap<QString, QVariantList> idsPerRepository;
//    stream >> idsPerRepository;

//    for (auto it = idsPerRepository.cbegin(); it != idsPerRepository.cend();
//    ++it)
//    {
//        auto repository = it.key();
//        auto allRepositoryEvent = retrieveEvents(repository);
//        for (auto uuid : it.value())
//        {
//            for (auto repositoryEvent : allRepositoryEvent)
//            {
//                if (uuid.toUuid() == repositoryEvent->getUniqId())
//                {
//                    events << repositoryEvent;
//                }
//            }
//        }
//    }

//    return events;
//}

// QByteArray CatalogueController::mimeDataForCatalogues(
//    const QVector<std::shared_ptr<DBCatalogue>>& catalogues) const
//{
//    auto encodedData = QByteArray {};

//    QMap<QString, QVariantList> idsPerRepository;
//    for (auto catalogue : catalogues)
//    {
//        idsPerRepository[catalogue->getRepository()] <<
//        catalogue->getUniqId();
//    }

//    QDataStream stream { &encodedData, QIODevice::WriteOnly };
//    stream << idsPerRepository;

//    return encodedData;
//}

// QVector<std::shared_ptr<DBCatalogue>>
// CatalogueController::cataloguesForMimeData(
//    const QByteArray& mimeData) const
//{
//    auto catalogues = QVector<std::shared_ptr<DBCatalogue>> {};
//    QDataStream stream { mimeData };

//    QMap<QString, QVariantList> idsPerRepository;
//    stream >> idsPerRepository;

//    for (auto it = idsPerRepository.cbegin(); it != idsPerRepository.cend();
//    ++it)
//    {
//        auto repository = it.key();
//        auto allRepositoryCatalogues = retrieveCatalogues(repository);
//        for (auto uuid : it.value())
//        {
//            for (auto repositoryCatalogues : allRepositoryCatalogues)
//            {
//                if (uuid.toUuid() == repositoryCatalogues->getUniqId())
//                {
//                    catalogues << repositoryCatalogues;
//                }
//            }
//        }
//    }

//    return catalogues;
//}

// void CatalogueController::initialize()
//{
//    qCDebug(LOG_CatalogueController())
//        << tr("CatalogueController init") << QThread::currentThread();

//    impl->m_CatalogueDao.initialize();
//    auto defaultRepositoryLocation
//        = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

//    QDir defaultRepositoryLocationDir;
//    if (defaultRepositoryLocationDir.mkpath(defaultRepositoryLocation))
//    {
//        defaultRepositoryLocationDir.cd(defaultRepositoryLocation);
//        auto defaultRepository =
//        defaultRepositoryLocationDir.absoluteFilePath(REPOSITORY_DEFAULT);

//        qCInfo(LOG_CatalogueController())
//            << tr("Persistant data loading from: ") << defaultRepository;

//        QDir dbDir(defaultRepository);
//        impl->m_RepositoryList << REPOSITORY_DEFAULT;
//        if (dbDir.exists())
//        {
//            auto dirName = dbDir.dirName();

//            if (impl->m_CatalogueDao.addDB(defaultRepository, dirName))
//            {
//                impl->copyDBtoDB(dirName, impl->toWorkRepository(dirName));
//            }
//        }
//        else
//        {
//            qCInfo(LOG_CatalogueController())
//                << tr("Initialisation of Default repository detected") <<
//                defaultRepository;
//        }
//    }
//    else
//    {
//        qCWarning(LOG_CatalogueController())
//            << tr("Cannot load the persistent default repository from ")
//            << defaultRepositoryLocation;
//    }

//    qCDebug(LOG_CatalogueController()) << tr("CatalogueController init END");
//}

// QString CatalogueController::CatalogueControllerPrivate::eventUniqueKey(
//    const std::shared_ptr<DBEvent>& event) const
//{
//    return event->getUniqId().toString().append(event->getRepository());
//}

// QString CatalogueController::CatalogueControllerPrivate::catalogueUniqueKey(
//    const std::shared_ptr<DBCatalogue>& catalogue) const
//{
//    return
//    catalogue->getUniqId().toString().append(catalogue->getRepository());
//}

// void CatalogueController::CatalogueControllerPrivate::copyDBtoDB(
//    const QString& dbFrom, const QString& dbTo)
//{
//    //    auto cataloguesShared = std::list<std::shared_ptr<DBCatalogue> >{};
//    auto catalogues = m_CatalogueDao.getCatalogues(dbFrom);
//    auto events = m_CatalogueDao.getEvents(dbFrom);
//    for (auto catalogue : catalogues)
//    {
//        m_CatalogueDao.copyCatalogue(catalogue, dbTo, true);
//    }

//    for (auto event : events)
//    {
//        m_CatalogueDao.copyEvent(event, dbTo, true);
//    }
//}

// QString
// CatalogueController::CatalogueControllerPrivate::toWorkRepository(QString
// repository)
//{
//    auto syncRepository = toSyncRepository(repository);

//    return QString("%1%2").arg(syncRepository, REPOSITORY_WORK_SUFFIX);
//}

// QString
// CatalogueController::CatalogueControllerPrivate::toSyncRepository(QString
// repository)
//{
//    auto syncRepository = repository;
//    if (repository.endsWith(REPOSITORY_WORK_SUFFIX))
//    {
//        syncRepository.remove(REPOSITORY_WORK_SUFFIX);
//    }
//    else if (repository.endsWith(REPOSITORY_TRASH_SUFFIX))
//    {
//        syncRepository.remove(REPOSITORY_TRASH_SUFFIX);
//    }
//    return syncRepository;
//}

// void CatalogueController::CatalogueControllerPrivate::savAllDB()
//{
//    for (auto repository : m_RepositoryList)
//    {
//        auto defaultRepositoryLocation
//            =
//            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
//        m_CatalogueDao.saveDB(defaultRepositoryLocation, repository);
//    }
//}

// void CatalogueController::CatalogueControllerPrivate::saveEvent(
//    std::shared_ptr<DBEvent> event, bool persist)
//{
//    m_CatalogueDao.copyEvent(*event, toSyncRepository(event->getRepository()),
//    true); if (persist)
//    {
//        savAllDB();
//    }
//}

// void CatalogueController::CatalogueControllerPrivate::saveCatalogue(
//    std::shared_ptr<DBCatalogue> catalogue, bool persist)
//{
//    m_CatalogueDao.copyCatalogue(*catalogue,
//    toSyncRepository(catalogue->getRepository()), true); if (persist)
//    {
//        savAllDB();
//    }
//}

// std::shared_ptr<IRequestPredicate>
// CatalogueController::CatalogueControllerPrivate::createFinder(
//    const QUuid& uniqId, const QString& repository, DBType type)
//{
//    // update catalogue parameter
//    auto uniqIdPredicate = std::make_shared<ComparaisonPredicate>(
//        QString { "uniqId" }, uniqId, ComparaisonOperation::EQUALEQUAL);

//    auto repositoryType = repository;
//    switch (type)
//    {
//        case DBType::SYNC:
//            repositoryType = toSyncRepository(repositoryType);
//            break;
//        case DBType::WORK:
//            repositoryType = toWorkRepository(repositoryType);
//            break;
//        case DBType::TRASH:
//        default:
//            break;
//    }

//    auto repositoryPredicate = std::make_shared<ComparaisonPredicate>(
//        QString { "repository" }, repositoryType,
//        ComparaisonOperation::EQUALEQUAL);

//    auto finderPred =
//    std::make_shared<CompoundPredicate>(CompoundOperation::AND);
//    finderPred->AddRequestPredicate(uniqIdPredicate);
//    finderPred->AddRequestPredicate(repositoryPredicate);

//    return finderPred;
//}
