#include "filenameparser.h"

#include <QHash>
#include <QRegExp>
#include <QtDebug>

#include <numeric>

const int FilenameParser::kSuffixRemovalCount = 3;
const float FilenameParser::kSuffixRemovalThreshold = 0.5;

static int AccumulateLength(int initial, const QString& str) {
  return initial + str.length();
}

static bool CompareLength(const QStringList& left, const QStringList& right) {
  return std::accumulate(left.begin(), left.end(), 0, AccumulateLength) >
         std::accumulate(right.begin(), right.end(), 0, AccumulateLength);
}

uint qHash(const QStringList& v) {
  return qHash(v.join(""));
}


template <typename AddFunction>
void AccumulateXfix(AddFunction add, const QStringList& parts, int dir,
                    QHash<QStringList, int>* accumulator) {
  const int start = dir > 0 ? 0 : parts.count()-1;

  QStringList temp;
  for (int i=start ; i<parts.count() && i>=0 ; i+=dir) {
    ((temp).*(add))(parts[i]);
    (*accumulator)[temp] ++;
  }
}

void RemoveXfix(const QHash<QStringList, int>& accumulator, bool prefix,
                QStringList* filenames) {
  QList<QStringList> things_to_remove;
  for (QHash<QStringList, int>::const_iterator it = accumulator.constBegin();
       it != accumulator.constEnd() ; ++it) {
    if (float(it.value()) / filenames->count() >= FilenameParser::kSuffixRemovalThreshold) {
      things_to_remove << it.key();
    }
  }

  qSort(things_to_remove.begin(), things_to_remove.end(), CompareLength);

  foreach (const QStringList& nfix, things_to_remove) {
    QString re_contents = prefix ? "^" : "$";
    foreach (const QString& part, nfix) {
      if (prefix) {
        re_contents.append(QRegExp::escape(part) + "\\W+");
      } else {
        re_contents.prepend("\\W+" + QRegExp::escape(part));
      }
    }
    QRegExp re(re_contents);

    for (int i=0 ; i<filenames->count() ; ++i) {
      (*filenames)[i].remove(re);
    }
  }
}


FilenameParser::FilenameParser(const QStringList& filenames) {
  QRegExp junk("\\b(?:"
      "320p|480p|720p|1080p|[xh]264|xvid|fqm|mspaint|dvb-crew|mrgspott|ws|web|"
      "ander|xor|fpn|lol|ct|swesub|proper|repack|\\w+tv|\\w+dvd|hv|aaf|bia|"
      "fqm|no-rar|nbs|avm|slashit|bwb|ctu|glt|dvdrip|tcm|syncfix|wat|saints|"
      "lmao|tracker|btarena|org|tz|caph|hiqt|xcrypt|bluray|sfm|sinners|2hd|"
      "immerse|orenji|ac3)\\b",
               Qt::CaseInsensitive);

  QHash<QStringList, int> common_prefixes;
  QHash<QStringList, int> common_suffixes;

  QList<QStringList> filename_parts;

  foreach (QString filename_copy, filenames) {
    // Remove the extension
    const int last_dot = filename_copy.lastIndexOf('.');
    if (last_dot != -1) {
      filename_copy.remove(last_dot, filename_copy.length());
    }

    // Try to strip some of the junk that's commonly found at the end of
    // filenames in legally acquired backups of TV shows.
    filename_copy.remove(junk);

    if (filenames.count() >= kSuffixRemovalCount) {
      // Find common suffixes and prefixes by splitting on non-word characters.
      const QStringList parts = filename_copy.split(QRegExp("\\W"),
                                                    QString::SkipEmptyParts);
      filename_parts << parts;

      AccumulateXfix<void (QList<QString>::*)(const QString&)>
                    (&QStringList::append, parts, +1, &common_prefixes);
      AccumulateXfix(&QStringList::prepend, parts, -1, &common_suffixes);
    }

    filenames_ << filename_copy;
  }

  // Remove prefixes and suffixes
  if (filenames.count() > kSuffixRemovalCount) {
    qDebug() << filenames_;
    RemoveXfix(common_prefixes, true, &filenames_);
    RemoveXfix(common_suffixes, false, &filenames_);
    qDebug() << filenames_;
  }
}

FilenameParser::Information FilenameParser::Parse(int i) const {
  Information ret;
  QString filename_copy(filenames_[i]);

  ParseSE(&filename_copy, &ret);

  if (ret.series_ == -1) {
    ParseS(&filename_copy, &ret);
  }
  if (ret.episode_ == -1) {
    ParseE(&filename_copy, &ret);
  }

  // Split what's left of the filename on word boundaries and add it to the name
  ret.name_ = filename_copy.split(QRegExp("[._-\\s\\[\\]\\(\\)\\<\\>]"), QString::SkipEmptyParts).join(" ");

  return ret;
}

void FilenameParser::ParseSE(QString* filename, Information* ret) const {
  QRegExp re("[sS](\\d+)[eE](\\d+)");
  if (re.indexIn(*filename) != -1) {
    ret->series_ = re.cap(1).toInt();
    ret->episode_ = re.cap(2).toInt();
    filename->remove(re.pos(), re.matchedLength());
    return;
  }

  re = QRegExp("(\\d+)[xX](\\d+)");
  if (re.indexIn(*filename) != -1) {
    ret->series_ = re.cap(1).toInt();
    ret->episode_ = re.cap(2).toInt();
    filename->remove(re.pos(), re.matchedLength());
    return;
  }
}

void FilenameParser::ParseS(QString* filename, Information* ret) const {
  QRegExp re("\\b(?:series|season)\\W*(\\d+)", Qt::CaseInsensitive);
  if (re.indexIn(*filename) != -1) {
    ret->series_ = re.cap(1).toInt();
    filename->remove(re.pos(), re.matchedLength());
    return;
  }
}

void FilenameParser::ParseE(QString* filename, Information* ret) const {
  QRegExp re("\\b(?:episode|part|pt)\\W*(\\d+)", Qt::CaseInsensitive);
  if (re.indexIn(*filename) != -1) {
    ret->episode_ = re.cap(1).toInt();
    filename->remove(re.pos(), re.matchedLength());
    return;
  }
}
