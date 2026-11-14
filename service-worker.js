// Tvarkaraščio darbas be interneto.
// Podėlyje laikomas tik pats puslapis ir jo failai – tvarkaraščio duomenis
// programėlė saugo atskirai (localStorage) ir be interneto rodo paskutinius
// sėkmingai nuskaitytus duomenis.
const CACHE_NAME = 'tvarkarastis-v1';

const APP_SHELL = [
    './index.html',
    './manifest.json',
    './icons/icon-192.png',
    './icons/icon-512.png',
    './icons/icon-maskable-512.png',
    './icons/apple-touch-icon.png'
];

self.addEventListener('install', (event) => {
    event.waitUntil(
        caches.open(CACHE_NAME)
            .then((cache) => cache.addAll(APP_SHELL))
            .then(() => self.skipWaiting())
    );
});

self.addEventListener('activate', (event) => {
    event.waitUntil(
        caches.keys()
            .then((keys) => Promise.all(
                keys.filter((key) => key !== CACHE_NAME).map((key) => caches.delete(key))
            ))
            .then(() => self.clients.claim())
    );
});

self.addEventListener('fetch', (event) => {
    const request = event.request;
    if (request.method !== 'GET') return;

    const url = new URL(request.url);
    // Google Sheets ir kiti svetimi adresai – visada iš tinklo.
    if (url.origin !== self.location.origin) return;

    // Puslapio atidarymas: pirmiausia tinklas (kad atnaujinimai matytųsi
    // iškart), o be interneto – podėlyje išsaugotas puslapis.
    if (request.mode === 'navigate') {
        event.respondWith(
            fetch(request)
                .then((response) => {
                    if (response.ok) {
                        const copy = response.clone();
                        caches.open(CACHE_NAME).then((cache) => cache.put('./index.html', copy));
                    }
                    return response;
                })
                .catch(() => caches.match('./index.html').then((cached) => cached || Response.error()))
        );
        return;
    }

    // Kiti to paties adreso failai (piktogramos, manifestas) – iš podėlio.
    event.respondWith(
        caches.match(request).then((cached) => {
            if (cached) return cached;
            return fetch(request).then((response) => {
                if (response.ok) {
                    const copy = response.clone();
                    caches.open(CACHE_NAME).then((cache) => cache.put(request, copy));
                }
                return response;
            });
        })
    );
});
