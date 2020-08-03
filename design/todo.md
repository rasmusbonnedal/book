Todo-list
=========

- [x] Icon for Application
- [x] Load Document
- [x] Save Document
- [x] Jump to next field
- [x] Make verifikat date editable
- [x] Only allow dates of the document year
- [x] Fix bug where a too high verifikat is selected when opening new doc
- [x] Fix bug where verifikat view gets huge when looking at verifikat 0 (Add scrollbars)
- [x] Suggest auto-balancing values
- [x] Fractional support
- [x] Refactor gtk views
- [x] Why does load take a long time? (Regex compilation!)
- [x] Why does not ingående saldon balance in 2019? (Pengar parse error for '-0.92')
- [x] Fix thousands separator
- [x] Edit numbers without separator
- [x] Start editing verifikat when pressing enter on text in grundbok
- [x] Start editing date of next verifikat when pressing enter on balanced verifikat
- [x] Fix bokdatum (not 0000-00-00)
- [x] Don't exit with unsaved changes
- [x] Right align pengar columns
- [x] Keyboard shortcuts
- [ ] Check bokföringsdatum on entries
- [ ] Disallow edit old entries
- [ ] Allow strykning of old entries
- [ ] Add/Edit konton
- [ ] Reports
  * [x] Saldon
  * [x] Resultaträkning
  * [ ] Balansräkning
  * [x] Taggrapport
  * [ ] Grundbok
  * [ ] Huvudbok
  * [ ] Filters
- [ ] Search
- [ ] Why are there two "2099 ansamlad vinst" in ingående saldon?
- [ ] Check reports for year 2019 against bollbok
  * [x] Saldon
  * [x] Resultaträkning
  * [ ] Balansräkning
  * [ ] Taggrapport
  * [ ] Grundbok
  * [ ] Huvudbok
- [x] About box with version, date and git rev #
- [x] Set date on new verifikat to last edited date
- [x] Use comma as decimal separator, accept both . and ,
- [ ] Accept verifikat only if balanced, and directly on enter on empty konto
- [ ] Show non-"normal" konto entries in red
- [x] Live saldo transaction view in sidebar
- [x] Right just pengar in Saldo view

XML diffs
*********
 - [x] \<kontoplan> add attribute kptyp=""
 - [x] space before '/>' (Kommer kräva custom printing)
 - [x] \<kontogrupper>\<kontogrupp namn="asdf" konton="1234,2345,3456">...
 - [x] \<objektlista>\</objektlista>
 - [x] pengar="123.60" should be pengar="123.6"

Needed for first entry
**********************
- [x] Save

For first month
***************
- [ ] "Konto" report
- [ ] "Tagg" report
