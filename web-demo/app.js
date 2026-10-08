const MONTHS = ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"];
const WEEKDAYS_MONDAY = ["Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"];
const WEEKDAYS_SUNDAY = ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];
let resourceRecords = [];

function leapYear(year) { return year % 4 === 0 && (year % 100 !== 0 || year % 400 === 0); }
function monthLength(year, month) { return [31, leapYear(year) ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31][month - 1] || 0; }
function daysFromCivil(year, month, day) {
  year -= month <= 2 ? 1 : 0;
  const era = Math.floor(year / 400);
  const yoe = year - era * 400;
  const adjusted = month + (month > 2 ? -3 : 9);
  const doy = Math.floor((153 * adjusted + 2) / 5) + day - 1;
  const doe = yoe * 365 + Math.floor(yoe / 4) - Math.floor(yoe / 100) + doy;
  return era * 146097 + doe - 719468;
}
function civilFromDays(value) {
  let z = value + 719468;
  const era = Math.floor(z / 146097);
  const doe = z - era * 146097;
  const yoe = Math.floor((doe - Math.floor(doe / 1460) + Math.floor(doe / 36524) - Math.floor(doe / 146096)) / 365);
  let year = yoe + era * 400;
  const doy = doe - (365 * yoe + Math.floor(yoe / 4) - Math.floor(yoe / 100));
  const mp = Math.floor((5 * doy + 2) / 153);
  const day = doy - Math.floor((153 * mp + 2) / 5) + 1;
  const month = mp + (mp < 10 ? 3 : -9);
  year += month <= 2 ? 1 : 0;
  return {year, month, day};
}
function weekdaySunday0(date) { return ((daysFromCivil(date.year, date.month, date.day) + 4) % 7 + 7) % 7; }
function isoWeek(date) {
  const mondayIndex = (weekdaySunday0(date) + 6) % 7;
  const monday = civilFromDays(daysFromCivil(date.year, date.month, date.day) - mondayIndex);
  const thursday = civilFromDays(daysFromCivil(monday.year, monday.month, monday.day) + 3);
  const jan4 = {year: thursday.year, month: 1, day: 4};
  const week1Monday = daysFromCivil(jan4.year, jan4.month, jan4.day) - ((weekdaySunday0(jan4) + 6) % 7);
  const number = Math.floor((daysFromCivil(monday.year, monday.month, monday.day) - week1Monday) / 7) + 1;
  return {year: thursday.year, number, monday, sunday: civilFromDays(daysFromCivil(monday.year, monday.month, monday.day) + 6)};
}
function monthCells(year, month, mondayFirst) {
  const offset = mondayFirst ? (weekdaySunday0({year, month, day: 1}) + 6) % 7 : weekdaySunday0({year, month, day: 1});
  const rows = Math.ceil((offset + monthLength(year, month)) / 7);
  const first = daysFromCivil(year, month, 1) - offset;
  return {rows, cells: Array.from({length: rows * 7}, (_, index) => civilFromDays(first + index))};
}
function addMonth(delta) {
  let year = Number(yearInput.value), month = Number(monthSelect.value) + delta;
  while (month < 1) { month += 12; year--; }
  while (month > 12) { month -= 12; year++; }
  if (year < 1 || year > 9999) return;
  monthSelect.value = String(month);
  yearInput.value = String(year);
  render();
}
function recordFor(year, month) { return resourceRecords.find(item => item.year === year && item.month === month && item.verified === true); }
function renderResource(year, month) {
  const action = document.getElementById("resource-action");
  const record = recordFor(year, month);
  action.replaceChildren();
  if (!record) {
    const note = document.createElement("div");
    note.className = "unavailable";
    note.textContent = "Printable reference unavailable for this selected year. The firmware calendar still works normally.";
    action.append(note);
    return;
  }
  const anchor = document.createElement("a");
  anchor.href = record.url;
  anchor.target = "_blank";
  anchor.rel = "noopener noreferrer";
  anchor.textContent = record.displayName;
  const arrow = document.createElement("span"); arrow.textContent = "↗"; arrow.setAttribute("aria-hidden", "true");
  anchor.append(arrow); action.append(anchor);
}
function renderSequence() {
  const sequence = document.getElementById("sequence");
  sequence.replaceChildren();
  resourceRecords.forEach(record => {
    const link = document.createElement("a");
    link.href = record.url; link.target = "_blank"; link.rel = "noopener noreferrer";
    const strong = document.createElement("strong"); strong.textContent = record.displayName;
    const small = document.createElement("small"); small.textContent = "Verified printable page ↗";
    link.append(strong, small); sequence.append(link);
  });
  [10, 11, 12].forEach(month => {
    const item = document.createElement("div"); item.className = "not-available";
    const strong = document.createElement("strong"); strong.textContent = `${MONTHS[month - 1]} 2027`;
    const small = document.createElement("small"); small.textContent = "Reference unavailable";
    item.append(strong, small); sequence.append(item);
  });
}
function render() {
  const year = Number(yearInput.value), month = Number(monthSelect.value), mondayFirst = mondayInput.checked;
  if (!Number.isInteger(year) || year < 1 || year > 9999 || !Number.isInteger(month) || month < 1 || month > 12) return;
  document.getElementById("calendar-heading").textContent = MONTHS[month - 1];
  document.getElementById("year-label").textContent = year;
  const cells = monthCells(year, month, mondayFirst);
  const weekdayLabels = mondayFirst ? WEEKDAYS_MONDAY : WEEKDAYS_SUNDAY;
  const calendar = document.getElementById("calendar"); calendar.replaceChildren();
  const weekColumn = document.getElementById("week-column"); weekColumn.replaceChildren();
  weekdayLabels.forEach(label => { const item = document.createElement("div"); item.className = "calendar-cell weekday"; item.setAttribute("role", "columnheader"); item.textContent = label; calendar.append(item); });
  cells.cells.forEach((date, index) => {
    if (index % 7 === 0) {
      const label = document.createElement("div"); label.textContent = isoWeek(date).number; label.title = `ISO week ${isoWeek(date).number}, ${isoWeek(date).year}`; weekColumn.append(label);
    }
    const cell = document.createElement("div");
    cell.className = "calendar-cell" + (date.month !== month ? " outside" : "");
    cell.setAttribute("role", "gridcell");
    cell.setAttribute("aria-label", `${MONTHS[date.month - 1]} ${date.day}, ${date.year}`);
    cell.textContent = date.day;
    calendar.append(cell);
  });
  document.getElementById("week-column").style.display = isoInput.checked ? "grid" : "none";
  document.getElementById("calendar").style.gridTemplateColumns = isoInput.checked ? "repeat(7,minmax(0,1fr))" : "repeat(7,minmax(0,1fr))";
  document.getElementById("selected-week").textContent = isoInput.checked ? `ISO WEEK ${isoWeek({year, month, day: 1}).number} / ${isoWeek({year, month, day: 1}).year}` : "WEEK LABELS OFF";
  document.getElementById("month-summary").textContent = `${monthLength(year, month)} days · ${cells.rows} natural rows`;
  renderResource(year, month);
}
const yearInput = document.getElementById("year");
const monthSelect = document.getElementById("month");
const mondayInput = document.getElementById("monday");
const isoInput = document.getElementById("iso");
MONTHS.forEach((name, index) => { const option = document.createElement("option"); option.value = index + 1; option.textContent = name; monthSelect.append(option); });
monthSelect.value = "1";
document.getElementById("previous").addEventListener("click", () => addMonth(-1));
document.getElementById("next").addEventListener("click", () => addMonth(1));
[yearInput, monthSelect, mondayInput, isoInput].forEach(element => element.addEventListener("input", render));
document.addEventListener("keydown", event => {
  if (event.target instanceof HTMLInputElement || event.target instanceof HTMLSelectElement) return;
  if (event.key === "ArrowLeft") addMonth(-1);
  else if (event.key === "ArrowRight") addMonth(1);
  else if (event.key === "ArrowUp" || event.key === "ArrowDown") {
    const year = Number(yearInput.value) + (event.key === "ArrowUp" ? 1 : -1);
    if (year >= 1 && year <= 9999) { yearInput.value = year; render(); }
  } else if (event.key.toLowerCase() === "w") { mondayInput.checked = !mondayInput.checked; render(); }
});
fetch("../data/calendar_resources.json").then(response => {
  if (!response.ok) throw new Error("Could not load resource manifest");
  return response.json();
}).then(manifest => {
  resourceRecords = manifest.resources.filter(item => item.status === "verified").map(item => ({
    year: item.year, month: item.month, displayName: item.display_name, url: item.url, verified: true
  }));
  renderSequence(); render();
}).catch(() => {
  document.getElementById("resource-copy").textContent = "Resource manifest could not load. Local calendar calculations remain available.";
  render();
});
