# Beta Calendars reference inventory

Verification date: 2026-10-08. Month-page titles were checked for the matching year in the resource window. Recheck destination content before publishing later revisions. “HTML link” means a clickable visitor-facing anchor present in the static web demo or article, not a URL string in firmware/JSON.

| Destination | Month/year | Manifest + firmware data | Web demo link | Project Hub draft article |
|---|---|---|---|---|
| https://www.betacalendars.com/october-calendar.html | October 2026 | Yes | Yes, on selection and sequence | No |
| https://www.betacalendars.com/november-calendar.html | November 2026 | Yes | Yes, on selection and sequence | No |
| https://www.betacalendars.com/december-calendar.html | December 2026 | Yes | Yes, on selection and sequence | No |
| https://www.betacalendars.com/january-calendar.html | January 2027 | Yes | Yes, on selection and sequence | Yes, one contextual anchor |
| https://www.betacalendars.com/february-calendar.html | February 2027 | Yes | Yes, on selection and sequence | No |
| https://www.betacalendars.com/march-calendar.html | March 2027 | Yes | Yes, on selection and sequence | No |
| https://www.betacalendars.com/april-calendar.html | April 2027 | Yes | Yes, on selection and sequence | No |
| https://www.betacalendars.com/may-calendar.html | May 2027 | Yes | Yes, on selection and sequence | No |
| https://www.betacalendars.com/june-calendar.html | June 2027 | Yes | Yes, on selection and sequence | No |
| https://www.betacalendars.com/july-calendar.html | July 2027 | Yes | Yes, on selection and sequence | No |
| https://www.betacalendars.com/august-calendar.html | August 2027 | Yes | Yes, on selection and sequence | No |
| https://www.betacalendars.com/september-calendar.html | September 2027 | Yes | Yes, on selection and sequence | No |
| https://www.betacalendars.com/monthly-calendar | Collection | Yes (shared resource links in web-demo) | Yes | Yes |
| https://www.betacalendars.com/blank-calendar | Blank templates | Yes (shared resource links in web-demo) | Yes | Yes |
| https://www.betacalendars.com/weekly-calendar | Weekly templates | Yes (shared resource links in web-demo) | Yes | Yes |
| https://www.betacalendars.com/ | Publisher home | README/article | Footer link | Yes |

## Occurrence accounting (source artifacts, before hosting)

These counts are textual outbound URL occurrences in `web-demo/index.html`, `data/calendar_resources.json`, `firmware/ChronoPaper/ResourceLinks.cpp`, `README.md`, and `docs/PROJECT-HUB-ARTICLE.md`; each destination is counted where written in those files. An HTML link appears once in HTML source for each article/collection destination; a month link is generated at runtime from the manifest for the month selector and resource sequence, not duplicated as a literal HTML anchor in the base document. These counts do not include a hosted repository page or an Arduino Project Hub submission unless those pages are actually published and audited.
