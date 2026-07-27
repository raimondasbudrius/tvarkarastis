#!/usr/bin/env python3
import http.server
import socketserver
import urllib.request
import json

class CORSProxy(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path.startswith('/api/schedule'):
            try:
                # Extract Google Sheets URL from path
                parts = self.path.split('?')
                if len(parts) > 1:
                    sheet_url = parts[1].replace('url=', '')
                    # Fetch from Google Sheets
                    req = urllib.request.Request(sheet_url)
                    req.add_header('User-Agent', 'Mozilla/5.0')
                    
                    with urllib.request.urlopen(req) as response:
                        data = response.read().decode('utf-8')
                    
                    self.send_response(200)
                    self.send_header('Content-type', 'application/json')
                    self.send_header('Access-Control-Allow-Origin', '*')
                    self.end_headers()
                    self.wfile.write(data.encode())
                else:
                    self.send_response(400)
                    self.end_headers()
                    self.wfile.write(b'Missing URL parameter')
            except Exception as e:
                self.send_response(500)
                self.send_header('Content-type', 'text/plain')
                self.send_header('Access-Control-Allow-Origin', '*')
                self.end_headers()
                self.wfile.write(f'Error: {str(e)}'.encode())
        else:
            super().do_GET()

if __name__ == '__main__':
    PORT = 8888
    with socketserver.TCPServer(('', PORT), CORSProxy) as httpd:
        print(f"Proxy server running at http://localhost:{PORT}")
        httpd.serve_forever()