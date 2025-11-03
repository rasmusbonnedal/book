import datetime
from pathlib import Path

from borb.pdf import Document, FixedColumnWidthTable, FlexibleColumnWidthTable, Heading, Image, LayoutElement, Page, Paragraph, PDF, SingleColumnLayout
import borb.pdf
import jinja2
from openpyxl import load_workbook

company = {
    'name': u'AB Schmaktiebolaget',
    'org_nr': '165560269986',
    'contact_name': u'Alf Orm',
    'contact_tel': '0733123456',
    'contact_email': 'alf.orm@schmaktie.se',
    'address': u'Bofinkstigen 33',
    'ort': u'Gnesta'
}

employee = {
    'name': u'Ben Burt',
    'id': '198612312382',
    'account': '5169-0012345'
}

def read_file(input_file):
    workbook = load_workbook(input_file, data_only=True, read_only=True)
    sheet = workbook['MyrPay']
    months = list(sheet.iter_rows(min_row=7, max_row=18, max_col=8, values_only=True))
    vab = sheet.iter_rows(min_row=21, max_col=2, values_only=True)
    vab = [x for x in vab if x[0] is not None]
    return (months, vab)

def skv_avrunda_nedat(belopp):
    return int(belopp)

def is_last(iter):
    l = len(iter)
    for i in range(l):
        yield iter[i], i == (l - 1)

def format_kr(belopp):
    s = f'{belopp:.2f} kr'
    x = 9
    while True:
        a = len(s) - x
        if a <= 0:
            break
        s = s[:a] + ' ' + s[a:]
        x += 4
    return s

def manads_namn(month):
    return ['januari', 'februari', 'mars', 'april', 'maj', 'juni', 'juli', 'augusti', 'september', 'oktober', 'november', 'december'][month-1]

# month to generate report for [1, 12]
def report(months, vab, year, month):
    if month < 1 or month > 12:
        raise Exception('Invalid month')
    m = months[month - 1]
    vab_month = month - 1 if month > 1 else 12
    current_vab = [x for x in vab if x[0].month == vab_month]

    month_name = m[0]
    brutto_lon = m[1]
    avdrag_vab = m[2]
    avdrag_skatt = skv_avrunda_nedat(m[3])
    bilforman = m[4] + m[5]
    nettolon_avdrag = m[5]
    traktamente = m[6] != None
    milersattning = m[7] != None
    brutto_kontant = brutto_lon + avdrag_vab
    utbetala = brutto_lon + avdrag_vab + avdrag_skatt + nettolon_avdrag

    hours = sum([y for _,y in current_vab])
    dates = ', '.join([f'{x.day}/{x.month}' for x, y in current_vab])
    if (hours == 0) ^ (avdrag_vab == 0):
        raise Exception(f'Error: vab-timmar: {hours}, vab-avdrag: {avdrag_vab}')
    if hours == 0:
        vab_spec = ''
    else:
        vab_spec = f' {hours} h ({dates})'

    borb.pdf.license.usage_statistics.UsageStatistics.opt_out()
    doc = Document()
    page = Page()
    doc.append_page(page)
    layout = SingleColumnLayout(page)
    layout.append_layout_element(FixedColumnWidthTable(1, 2, column_widths=[4, 1])
                                 .append_layout_element(Heading(f'Lönespecifikation {company["name"]}', font='Helvetica-bold', font_size=16, vertical_alignment=LayoutElement.VerticalAlignment.TOP))
                                 .append_layout_element(Image(Path('company.jpg'), size=(64, 64), horizontal_alignment=LayoutElement.HorizontalAlignment.RIGHT))
                                 .no_borders())

    table = FlexibleColumnWidthTable(3, 2)
    table = (table.append_layout_element(Paragraph('Namn', padding_right=20, font='Helvetica-bold'))
                  .append_layout_element(Paragraph(employee['name']))
                  .append_layout_element(Paragraph('Bankkonto', padding_right=20, font='Helvetica-bold'))
                  .append_layout_element(Paragraph(employee['account']))
                  .append_layout_element(Paragraph('Period', padding_right=20, font='Helvetica-bold'))
                  .append_layout_element(Paragraph(f'{year:04d}-{month:02d}')).no_borders())
    layout.append_layout_element(table)

    layout.append_layout_element(Paragraph('', padding_top=30))

    table_data = []
    table_data.append((f'Månadslön {manads_namn(month)}', format_kr(brutto_lon)))
    if avdrag_vab != 0:
        table_data.append((f'Avdrag VAB{vab_spec}', format_kr(avdrag_vab)))
        table_data.append((f'Summa bruttolön', format_kr(brutto_kontant)))
    table_data.append((f'Avdragen skatt', format_kr(avdrag_skatt)))
    if nettolon_avdrag != 0:
        table_data.append((f'Nettolöneavdrag bilförmån', format_kr(nettolon_avdrag)))
    if traktamente or milersattning:
        if traktamente and milersattning:
            spec = 'Traktamente och milersättning'
        elif traktamente:
            spec = 'Traktamente'
        else:
            spec = "Milersättning"
        table_data.append((f'{spec} har betalats ut under månaden', ''))
    table_data.append((f'Att utbetala', format_kr(utbetala)))

    table = FixedColumnWidthTable(len(table_data), 2, column_widths=[4, 1]).striped()
    first = True
    for d, isl in is_last(table_data):
        padding_top = 80 if isl else 5 if first else 0
        padding_bottom = 5 if isl else 0
        padding_side = 10
        first = False
        table.append_layout_element(Paragraph(d[0], padding_left=padding_side, padding_top=padding_top, padding_bottom=padding_bottom, ))
        table.append_layout_element(Paragraph(d[1], horizontal_alignment=LayoutElement.HorizontalAlignment.RIGHT, padding_right=padding_side, padding_top=padding_top, padding_bottom=padding_bottom))
    layout.append_layout_element(table.no_internal_borders())

    PDF.write(what=doc, where_to=f'lonespec-{manads_namn(month)}-{year}.pdf')

    if avdrag_skatt > 0:
        raise Exception('Avdragen skatt ska vara negativ')

    franvaro_uppgifter = []
    for vab in current_vab:
        franvaro_uppgifter.append({
            'BetalningsmottagarId': employee['id'],
            'FranvaroDatum': vab[0].strftime("%Y-%m-%d"),
            'FranvaroProcentTFP': int(vab[1] * 100 / 8),
            'FranvaroTimmarTFP': vab[1]
        })

    skv_brutto_kontant = skv_avrunda_nedat(brutto_kontant)
    skv_avdrprelskatt = skv_avrunda_nedat(-avdrag_skatt)
    iu = {
        'BetalningsmottagarId': employee['id'],
        'AvdrPrelSkatt': skv_avdrprelskatt,
        'KontantErsattningUlagAG': skv_brutto_kontant,
        'SkatteplOvrigaFormanerUlagAG': 0,
        'SkatteplBilformanUlagAG': skv_avrunda_nedat(bilforman),
        'DrivmVidBilformanUlagAG': 0,
        'AndraKostnadsers': 0,
        'BilErsattning' : milersattning,
        'Traktamente': traktamente,
        'ArbetsplatsensGatuadress': company['address'],
        'ArbetsplatsensOrt': company['ort'],
        'franvaro_uppgifter': franvaro_uppgifter
    }

    data = {
        'org_nummer': company['org_nr'],
        'kontakt_namn': company['contact_name'],
        'kontakt_tel_nummer': company['contact_tel'],
        'kontakt_email': company['contact_email'],
        'skapad_tid': datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S"),
        'period': f'{year:04d}{month:02d}',
        'total_arbavg': skv_avrunda_nedat(skv_brutto_kontant * 0.3142),
        'total_skatteavdr': skv_avdrprelskatt,
        'ius': [iu]
    }

    env = jinja2.Environment(loader=jinja2.FileSystemLoader('.'), undefined=jinja2.StrictUndefined)
    template = env.get_template('agd.jinja', globals=data)
    with open(f'arbetsgivardeklaration-{manads_namn(month)}-{year}.xml', 'w', encoding='utf-8') as f:
        print(template.render(), file=f)


input_file = r'myrpay.xlsx'
months, vab = read_file(input_file)

report(months, vab, 2025, 11)
