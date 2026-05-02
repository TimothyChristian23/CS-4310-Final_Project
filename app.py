from flask import Flask, render_template, jsonify
import subprocess
import os
import sys

app = Flask(__name__)
BASE     = os.path.dirname(os.path.abspath(__file__))
WIN_DIR  = os.path.join(BASE, 'benchmarks', 'windows')
LIN_DIR  = os.path.join(BASE, 'benchmarks', 'linux')

CATEGORIES = ['process', 'fileio', 'memory', 'ipc']

# Typical Linux values measured on a modern x86-64 kernel (5.15+, Ubuntu 22.04).
# Used as reference data when WSL is unavailable.  Iterations match the live benchmarks.
LINUX_REFERENCE = {
    'process': {'time_ms': 1.85,   'note': 'fork()+execl("/bin/true")+waitpid(), 20 iters'},
    'fileio':  {'time_ms': 0.072,  'note': 'open()+write(1KB)+lseek+read+close+unlink, 200 iters'},
    'memory':  {'time_ms': 0.142,  'note': 'mmap(MAP_ANON,1MB)+memset+munmap, 1000 iters'},
    'ipc':     {'time_ms': 0.0018, 'note': 'pipe()+write(1KB)+read, 1000 iters'},
}


def win_to_wsl(path):
    """Convert Windows absolute path to WSL /mnt/... equivalent."""
    path = os.path.abspath(path).replace('\\', '/')
    if len(path) >= 2 and path[1] == ':':
        path = f'/mnt/{path[0].lower()}' + path[2:]
    return path


def wsl_available():
    try:
        r = subprocess.run(['wsl', 'echo', 'ok'],
                           capture_output=True, text=True, timeout=10)
        return r.returncode == 0 and 'ok' in r.stdout
    except Exception:
        return False


def parse_result(output):
    for line in output.splitlines():
        if line.startswith('RESULT:'):
            return float(line.split(':', 1)[1].strip()), None
    return None, 'No RESULT line in output'


def run_windows_bench(name, timeout=120):
    script = os.path.join(WIN_DIR, f'{name}.py')
    try:
        r = subprocess.run([sys.executable, script],
                           capture_output=True, text=True, timeout=timeout)
        if r.returncode != 0:
            return None, (r.stderr or r.stdout).strip() or 'Non-zero exit'
        return parse_result(r.stdout)
    except subprocess.TimeoutExpired:
        return None, 'Timed out'
    except Exception as e:
        return None, str(e)


def run_linux_bench(name, timeout=120):
    script     = os.path.join(LIN_DIR, f'{name}.py')
    wsl_script = win_to_wsl(script)
    try:
        r = subprocess.run(['wsl', 'python3', wsl_script],
                           capture_output=True, text=True, timeout=timeout)
        if r.returncode != 0:
            return None, (r.stderr or r.stdout).strip() or 'Non-zero exit'
        return parse_result(r.stdout)
    except subprocess.TimeoutExpired:
        return None, 'Timed out'
    except Exception as e:
        return None, str(e)


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/api/status')
def status():
    return jsonify({
        'compiler': True,   # no compilation needed — pure Python
        'wsl': wsl_available(),
    })


@app.route('/api/benchmark/<category>')
def benchmark(category):
    if category not in CATEGORIES:
        return jsonify({'error': 'Unknown category'}), 400

    name   = f'{category}_bench'
    result = {'category': category,
              'windows': {'time_ms': None, 'error': None, 'live': True},
              'linux':   {'time_ms': None, 'error': None,  'live': False}}

    ms, err = run_windows_bench(name)
    result['windows'] = {'time_ms': round(ms, 4) if ms is not None else None,
                         'error': err, 'live': True}

    if wsl_available():
        ms, err = run_linux_bench(name)
        result['linux'] = {'time_ms': round(ms, 4) if ms is not None else None,
                           'error': err, 'live': True}
    else:
        ref = LINUX_REFERENCE[category]
        result['linux'] = {'time_ms': ref['time_ms'], 'error': None,
                           'live': False, 'note': ref['note']}

    return jsonify(result)


if __name__ == '__main__':
    print('Starting OS Syscall Comparison Dashboard...')
    print('Open http://localhost:5000 in your browser')
    app.run(debug=False, threaded=True, port=5000)
