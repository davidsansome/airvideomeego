#ifndef FILENAMEPARSER_H
#define FILENAMEPARSER_H

#include <QStringList>


class FilenameParser {
public:
  FilenameParser(const QStringList& filenames = QStringList());

  static const int kSuffixRemovalCount;
  static const float kSuffixRemovalThreshold;

  struct Information {
    Information() : series_(-1), episode_(-1) {}

    int series_;
    int episode_;
    QString name_;
  };

  Information Parse(int i) const;

private:
  void ParseSE(QString* filename, Information* ret) const;
  void ParseS(QString* filename, Information* ret) const;
  void ParseE(QString* filename, Information* ret) const;

private:
  QStringList filenames_;
};

#endif // FILENAMEPARSER_H
