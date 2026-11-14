# tvarkarastis
Molėtų progimnazijos pamokų tvarkaraštis

## Keitimų paryškinimas (geltoni langeliai → oranžinis paryškinimas)

Programa nuskaito langelių **fonines spalvas** iš Google Sheets lentelės per
Google Sheets API v4. Originalioje lentelėje geltonu fonu pažymėtos pamokos
(keitimai) tvarkaraštyje parodomos oranžiniu paryškinimu, o legenda paaiškina
spalvą („Keitimas“).

CSV/gviz endpoint'ai spalvų negrąžina, todėl reikalingas nemokamas API raktas:

1. Eikite į https://console.cloud.google.com/ ir susikurkite projektą.
2. „APIs & Services“ → „Library“ → įjunkite **Google Sheets API**.
3. „APIs & Services“ → „Credentials“ → „Create credentials“ → **API key**.
4. `index.html` faile įklijuokite raktą į konstantą `GOOGLE_API_KEY`.

Pastabos:
- Be rakto programa veikia įprastai, tik keitimai nebus paryškinti.
- Lentelė turi būti bendrinama „visi, turintys nuorodą“ (kaip ir dabar).
- „Publish to web“ (2PACX) ID spalvų nuskaitymo nepalaiko.
- Geltonos spalvos aptikimas atpažįsta visus Google Sheets paletės geltonus
  atspalvius (#ffff00, #fff2cc, #ffe599, #ffd966, #f1c232 ir pan.).

## Savaitės tipas ir darbas be interneto

- Savaitė (lyginė / nelyginė) skaičiuojama automatiškai nuo rugsėjo 1 d., ir
  visada pagal **rodomos dienos** savaitę.
- Savaitės dienos mygtukai neperšoka į kitą savaitę: trečiadienį paspaudus
  pirmadienį rodoma **šios** savaitės pirmadienio data ir tvarkaraštis (jau
  praėjusi diena). Savaitgalį ir po penktadienio pamokų aktuali jau ateinanti
  savaitė, todėl rodomas artimiausias pirmadienis su kitos savaitės tvarkaraščiu.
- Jei po atostogų mokykla pastumia savaites, tipą galima priverstinai nustatyti
  nuorodoje: `index.html?savaites=lygine` arba `index.html?savaites=nelygine`
  (tada viršuje rašoma „nustatyta rankiniu būdu“).
- Nutrūkus internetui rodomi paskutiniai sėkmingai nuskaityti duomenys su
  įspėjimu „Rodomi paskutiniai išsaugoti duomenys (nuo …)“, o ne klaidos
  pranešimas. Juos naršyklė saugo 30 dienų.
- Puslapiui nebereikia Bootstrap CDN: kraunamasis ratukas įrašytas pačiame
  faile, todėl tvarkaraštis atsidaro ir be interneto (jei duomenys jau buvo
  nuskaityti anksčiau).

## Technologijų pamokų medžiaga (micro:bit)

### `pamokos/7-1/` – 7 klasė · 1 ciklas „Nematoma arfa“ (2 pamokos)

Pagrindinė, **7 klasei** pritaikyta medžiaga. Schemos sudarytos **tik iš tų detalių, kurias realiai
turime**: micro:bit, Sharp GP2Y0A21 atstumo jutiklis, **2N3904** tranzistorius, **470 Ω** rezistorius,
garsiakalbis, **B50K** potenciometras ir **trys mygtukai**.

| Pamoka | Tema | Rezultatas ir vertinimas |
|---|---|---|
| **1** | Pažink micro:bit: kaip įkelti kodą (MakeCode), kaip multimetru išmatuoti įtampą, ką daro rezistorius | **Užpildyta internetinė forma** (1–2 mokinių vardai + matavimai) → el. paštas mokytojui → pažymys |
| **2** | Nematoma arfa: tranzistorinis stiprintuvas, garsumo potenciometras, 3 mygtukai, kalibravimas ir programavimas | Grojantis instrumentas + rubrika (pasirinktinai – 2 forma) |

- [`pamokos/7-1/README.md`](pamokos/7-1/README.md) – **ciklo aprašas**: komplektas grupei, kontaktų
  žemėlapis, pasiruošimas, ką spausdinti
- [`pamokos/7-1/1-pamoka.html`](pamokos/7-1/1-pamoka.html) ir
  [`2-pamoka.html`](pamokos/7-1/2-pamoka.html) – **pamokų planai** (spausdinami)
- [`pamokos/7-1/1-darbalapis.html`](pamokos/7-1/1-darbalapis.html) ir
  [`2-darbalapis.html`](pamokos/7-1/2-darbalapis.html) – **mokinių darbalapiai**
- [`pamokos/7-1/google-forma.md`](pamokos/7-1/google-forma.md) – formų laukai, el. pašto pranešimai
  mokytojui, Apps Script scenarijus, vertinimo susiejimas
- [`pamokos/7-1/kodas/`](pamokos/7-1/kodas/) – schemos, BOM, gedimų šalinimas ir programos
  (`.py` – MicroPython, `.js` – MakeCode, `3_radijo_duetas.py` – plėtra)

**Kontaktai:** `P0` – garsas → B50K → 470 Ω → 2N3904 bazė · `P1` – jutiklio `Vo` ·
`P8` – groti/tylėti · `P12` – oktava ↑ · `P16` – oktava ↓ (mygtukai jungiami į GND, kode įjungtas
vidinis pull-up). **Maitinimas – tik micro:bit 3 V** (USB laidas iš kompiuterio arba USB power
bankas): iš tos pačios linijos energiją gauna ir jutiklis, ir garsiakalbis, todėl **baterijų
dėklo nereikia**, o visur yra viena **bendra žemė**. Plėtros: akselerometras → garso aukštis,
radijo duetas (siųstuvas / klausytojas).

### Platesnė versija 6–8 kl. (archyvas)

Projektas **„Nematoma arfa“** – bekontaktis muzikos instrumentas (2 pamokos, 6–7 kl.):

- [`pamoka-arfa.html`](pamoka-arfa.html) – **pamokų planas** (13 skirsnių, spausdinamas)
- [`worksheet-arfa-kalibravimas.html`](worksheet-arfa-kalibravimas.html) – **mokinio darbalapis**
  (jungimo patikra, kalibravimo lentelė, grafikas, klausimai, „išėjimo bilietas“, savivertė)
- [`lesson-plan-air-harp.md`](lesson-plan-air-harp.md) – tas pats planas anglų kalba
- [`starter/air_harp.py`](starter/air_harp.py) – **MicroPython** pradinis kodas
- [`starter/README.md`](starter/README.md) – schema, kontaktų žemėlapis, komplektacija, gedimų šalinimas

Šioje versijoje papildomai aprašyti pjezo diskas, džoistikas, TTP223 lietimo jutiklis, BC547,
**baterijų dėklas 3× AA (4,5 V)** ir kiti komponentai – 7 klasės pamokoms naudokite
**`pamokos/7-1/`** aplanką.

> 🔋 **Svarbu apie maitinimą.** Archyvinė versija jutiklį maitina iš atskiro **4,5 V** dėklo.
> Naujoji `pamokos/7-1/` versija to **nedaro** – viskas (jutiklis, stiprintuvas, garsiakalbis)
> maitinama iš micro:bit **3 V** kontakto. Jei imate idėjas iš archyvo, laidus junkite pagal
> `pamokos/7-1/kodas/README.md` schemas.

2 pamokoje garsiakalbio garsumą valdo **B50K** (50 kΩ, tiesinis) potenciometras, jungiamas
stiprintuvo **signalo** pusėje (P0 → kraštinis išvadas, GND → kitas kraštinis, šluotelė → rezistorius →
tranzistoriaus bazė) – tam nereikia nei kodo, nei laisvo kontakto.

Failai atidaromi naršyklėje kaip ir tvarkaraštis (spausdinimui internetas nebūtinas).


