#include <QtWidgets>
#include <QtXml>

class BookDocument
{
public:
    bool open(const QString& filename);
    bool save(const QString& filename);


private:
    static bool calcChecksum(const QString& xmlText, quint32& result);
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

    file.seek(0);
    QTextStream fileStream(&file);
    quint32 checksum;
    if (!calcChecksum(fileStream.readAll(), checksum))
    {
        QMessageBox::warning(0, "Error", "Error calculating checksum");
        return false;
    }

    bool status;
    uint fileChecksum = m_document.elementsByTagName("bollbok").item(0).attributes()
                                  .namedItem("kontrollsumma").toAttr().value().toUInt(&status, 16);
    if (!status || fileChecksum != checksum)
    {
        QMessageBox::warning(0, "Error", "Checksum mismatch");
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
    QString xmlText = m_document.toString();
    quint32 checksum;
    if (!calcChecksum(xmlText, checksum))
    {
        QMessageBox::warning(0, "Error", "Error calculating checksum");
        return false;
    }
    QString value;
    value.setNum(checksum, 16);
    int zeros = 8 - value.length();
    for (int i = 0; i < zeros; ++i) value.prepend('0');
    m_document.elementsByTagName("bollbok").item(0).attributes().namedItem("kontrollsumma")
              .toAttr().setValue(value.toUpper());
    output << m_document.toString();
    return true;
}

bool BookDocument::calcChecksum(const QString& xmlText, quint32& result)
{
    int start = xmlText.indexOf("<bollbok");
    if (start == -1) return false;
    start = xmlText.indexOf(">", start);
    if (start == -1) return false;
    start += 1;
    int end = xmlText.indexOf("</bollbok>");
    if (end == -1) return false;
    QString subst = xmlText.mid(start, end - start);
    const ushort* data = subst.utf16();
    quint32 checksum = 1;
    int i = 0;
    int sum = 0;
    while (true)
    {
        if (i == 8 || *data == 0)
        {
            checksum = (checksum + sum) * 0x6f4f53 + 0xb7f;
            if (*data == 0) break;
            sum = 0;
            i = 0;
        }
        sum += *data;
        data++;
        i++;
    }
    result = checksum;
    return true;
}

void openDoc()
{
    BookDocument b;
    if (!b.open("../docs/bok1.bollbok"))
    {
        return;
    }
    b.save("../docs/bok1_rewrite.bollbok");
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
