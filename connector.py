import http.server
import socketserver
import json
import subprocess
import os
from socketserver import ThreadingMixIn

PORT = 8080
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
EXE_PATH = os.path.join(BASE_DIR, "traffic_engine.exe")
JSON_PATH = os.path.join(BASE_DIR, "simulation.json")


class ThreadedTCPServer(ThreadingMixIn, socketserver.TCPServer):
    allow_reuse_address = True


class SimHandler(http.server.SimpleHTTPRequestHandler):

    def do_POST(self):

        if self.path != "/run_sim":
            self.send_error(404)
            return

        try:
            content_length = int(self.headers.get("Content-Length", 0))
            post_data = self.rfile.read(content_length)

            data = json.loads(post_data)
            sim_input = data.get("input", "")

            if not os.path.exists(EXE_PATH):
                raise FileNotFoundError(
                    "traffic_engine.exe not found. Compile backend.c first."
                )

            if os.path.exists(JSON_PATH):
                os.remove(JSON_PATH)

            process = subprocess.Popen(
                [EXE_PATH],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                cwd=BASE_DIR
            )

            stdout, stderr = process.communicate(input=sim_input)

            if process.returncode != 0:
                raise RuntimeError(
                    "C program failed:\n" + stderr
                )

            if not os.path.exists(JSON_PATH):
                raise RuntimeError(
                    "simulation.json was not generated."
                )

            with open(JSON_PATH, "r") as f:
                result = json.load(f)

            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.send_header("Access-Control-Allow-Origin", "*")
            self.end_headers()

            self.wfile.write(json.dumps(result).encode())

        except Exception as e:

            self.send_response(500)
            self.send_header("Content-Type", "application/json")
            self.end_headers()

            self.wfile.write(
                json.dumps({"error": str(e)}).encode()
            )


if __name__ == "__main__":

    os.chdir(BASE_DIR)

    print("Starting server at http://localhost:8080")

    with ThreadedTCPServer(
        ("localhost", PORT),
        SimHandler
    ) as server:

        server.serve_forever()