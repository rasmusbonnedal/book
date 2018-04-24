#include <QtWidgets>
#include <QtXml>

class BookDocument
{
public:
    bool open(const QString& filename);
    bool save(const QString& filename);


private:
    QDomDocument m_document;
};

bool BookDocument::open(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(0, "Error", "Error opening xml file");
        return false;
    }
    if (!m_document.setContent(&file))
    {
        QMessageBox::warning(0, "Error", "Error parsing xml file");
        return false;
    }
    return true;
}

bool BookDocument::save(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(0, "Error", "Error opening xml file for writing");
        return false;
    }
    QTextStream output(&file);
    output << m_document.toString();
    return true;
}

void openDoc()
{
    BookDocument b;
    if (!b.open("..\\docs\\bok2017.bollbok"))
    {
        return;
    }
    b.save("..\\docs\\bok2017_rewrite.bollbok");
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  openDoc();

  QWidget window;
  QTableWidget table;
  table.setColumnCount(4);
  table.setRowCount(2);
  table.verticalHeader()->setVisible(false);
  QStringList headers;
  headers << "Verifikat"
          << "Datum"
          << "Beskrivning"
          << "Omslutning";
  table.setHorizontalHeaderLabels(headers);

  QVBoxLayout layout;
  layout.setSpacing(0);
  layout.setContentsMargins(0, 0, 0, 0);
  window.setLayout(&layout);

  layout.addWidget(&table);

  window.show();
  window.setWindowTitle("Book");
  return app.exec();
}
