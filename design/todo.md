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
- [ ] Edit konton
- [ ] Reports
  * [x] Saldon
  * [ ] Resultaträkning
  * [ ] Balansräkning
  * [ ] Taggrapport
  * [ ] Grundbok
  * [ ] Huvudbok
- [ ] Search
- [ ] Why are there two "2099 ansamlad vinst" in ingående saldon?

XML diffs
*********
 - [x] <kontoplan> add attribute kptyp=""
 - [x] space before '/>' (Kommer kräva custom printing)
 - [x] <kontogrupper><kontogrupp namn="asdf" konton="1234,2345,3456">...
 - [x] <objektlista></objektlista>
 - [x] pengar="123.60" should be pengar="123.6"

Needed for first entry
**********************
1. Save
