#ifndef SCIQLOP_CATALOGUECONTROLLER_H
#define SCIQLOP_CATALOGUECONTROLLER_H

#include "CoreGlobal.h"

#include <Common/spimpl.h>
#include <Data/DateTimeRange.h>
#include <QLoggingCategory>
#include <QObject>
#include <QUuid>
#include <Repository.hpp>
#include <map>
#include <memory>

class DBCatalogue;
class DBEvent;
class DBEventProduct;

Q_DECLARE_LOGGING_CATEGORY(LOG_CatalogueController)

class DataSourceItem;
class Variable;

constexpr auto default_repo = "default";

/**
 * @brief The CatalogueController class aims to handle catalogues and event
 * using the CatalogueAPI library.
 */
class SCIQLOP_CORE_EXPORT CatalogueController : public QObject
{
  Q_OBJECT
public:
  using time_t        = double;
  using Product_t     = CatalogiCpp::Product<time_t>;
  using Repository_t  = CatalogiCpp::Repository<time_t>;
  using Event_t       = Repository_t::Event_t;
  using Event_ptr     = Repository_t::Event_ptr;
  using Catalogue_t   = Repository_t::Catalogue_t;
  using Catalogue_ptr = typename Repository_t::Catalogue_ptr;
  using uuid_t        = Repository_t::uuid_t;
  template<typename... _Args>
  static inline Catalogue_ptr make_catalogue_ptr(_Args&&... __args)
  {
    return Repository_t::make_catalogue_ptr(std::forward<_Args>(__args)...);
  }

  template<typename... _Args>
  static inline Event_ptr make_event_ptr(_Args&&... __args)
  {
    return Catalogue_t::make_event_ptr(std::forward<_Args>(__args)...);
  }

  explicit CatalogueController(QObject* parent = nullptr);
  virtual ~CatalogueController();

  QStringList repositories() const;
  void loadRepository(const QString& path, const QString& name);
  void saveRepository(const QString& path, const QString& name);

  std::vector<Event_ptr> events();
  std::vector<Event_ptr> events(const QString& repository);

  std::vector<Catalogue_ptr> catalogues();
  std::vector<Catalogue_ptr> catalogues(const QString& repository);

  bool hasUnsavedChanges(Event_ptr event);

  std::optional<QString> repository(Event_ptr event);
  std::optional<QString> repository(Catalogue_ptr catalogue);

  void save(Event_ptr event);
  void save(Catalogue_ptr catalogue);
  void save(const QString& repository);

  void add(const QString& repository);
  void add(const QString& catalogue, const QString& repository);
  void add(Event_ptr event, Catalogue_ptr catalogue);
  void add(Event_ptr event, const QString& repository = default_repo);

  //    // Event
  //    /// retrieveEvents with empty repository retrieve them from the default
  //    repository std::list<std::shared_ptr<DBEvent>> retrieveEvents(const
  //    QString& repository) const; std::list<std::shared_ptr<DBEvent>>
  //    retrieveAllEvents() const;

  //    void addEvent(std::shared_ptr<DBEvent> event);
  //    void updateEvent(std::shared_ptr<DBEvent> event);
  //    void updateEventProduct(std::shared_ptr<DBEventProduct> eventProduct);
  //    void removeEvent(std::shared_ptr<DBEvent> event);
  //    //    void trashEvent(std::shared_ptr<DBEvent> event);
  //    //        void restore(std::shared_ptr<DBEvent> event);
  //    void saveEvent(std::shared_ptr<DBEvent> event);
  //    void discardEvent(std::shared_ptr<DBEvent> event, bool& removed);
  //    bool eventHasChanges(std::shared_ptr<DBEvent> event) const;

  //    // Catalogue
  //    std::list<std::shared_ptr<DBEvent>> retrieveEventsFromCatalogue(
  //        std::shared_ptr<DBCatalogue> catalogue) const;

  //    /// retrieveEvents with empty repository retrieve them from the default
  //    repository std::list<std::shared_ptr<DBCatalogue>> retrieveCatalogues(
  //        const QString& repository = QString()) const;
  //    void addCatalogue(std::shared_ptr<DBCatalogue> catalogue);
  //    void updateCatalogue(std::shared_ptr<DBCatalogue> catalogue);
  //    void removeCatalogue(std::shared_ptr<DBCatalogue> catalogue);
  //    void saveCatalogue(std::shared_ptr<DBCatalogue> catalogue);
  //    void discardCatalogue(std::shared_ptr<DBCatalogue> catalogue, bool&
  //    removed);

  //    void saveAll();
  //    bool hasChanges() const;

  //    /// Returns the MIME data associated to a list of variables
  //    QByteArray mimeDataForEvents(const QVector<std::shared_ptr<DBEvent>>&
  //    events) const;

  //    /// Returns the list of variables contained in a MIME data
  //    QVector<std::shared_ptr<DBEvent>> eventsForMimeData(const QByteArray&
  //    mimeData) const;

  //    /// Returns the MIME data associated to a list of variables
  //    QByteArray mimeDataForCatalogues(const
  //    QVector<std::shared_ptr<DBCatalogue>>& catalogues) const;

  //    /// Returns the list of variables contained in a MIME data
  //    QVector<std::shared_ptr<DBCatalogue>> cataloguesForMimeData(const
  //    QByteArray& mimeData) const;

  // public slots:
  //    /// Manage init/end of the controller
  //    void initialize();

private:
  std::map<QString, CatalogiCpp::Repository<time_t>> _lastSavedRepos;
  std::map<QString, CatalogiCpp::Repository<time_t>> _currentRepos;

  //    class CatalogueControllerPrivate;
  //    spimpl::unique_impl_ptr<CatalogueControllerPrivate> impl;
};

#endif // SCIQLOP_CATALOGUECONTROLLER_H
