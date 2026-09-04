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
