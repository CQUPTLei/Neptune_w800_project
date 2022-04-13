import argparse
import os
import sys
import subprocess
import json
import hashlib

HELP_LINK = 'https://developer.huawei.com/consumer/cn/forum/topic/0203380024404140371?fid=0103702273237520029'

def check_file_sha256(src_path, sha_path):
    if not os.path.isfile(src_path) or not os.path.isfile(sha_path):
        return False
    with open(src_path, 'rb') as src_file:
        sha256 = hashlib.sha256()
        sha256.update(src_file.read())
        cal_value = sha256.hexdigest()
    with open(sha_path, 'r') as sha_file:
        hsh_value = sha_file.readline().strip()
    return cal_value == hsh_value


def download_tool(url, dst, tgt_dir=None):
    import requests
    try:
        res = requests.get(url, stream=True, timeout=(5, 9))
    except OSError:
        raise Exception(f'Download {url} timeout!')

    if res.status_code == 200:
        print(f'Downloading {url} ...')
    else:
        print(f'Downloading {url} failed with code: {res.status_code}!')
        return res.status_code

    total_size = int(res.headers['content-length'])
    download_size = 0
    download_percent = 0

    try:
        with open(dst, "wb") as f:
            for chunk in res.iter_content(chunk_size=1024):
                if chunk:
                    f.write(chunk)
                    download_size += len(chunk)
                    download_percent = round(
                        float(download_size / total_size * 100), 2)
                    print('Progress: %s%%\r' % download_percent, end=' ')
            print('Download complete!')
    except OSError:
        raise Exception(
            f'{url} download failed, please install it manually!')


def main():
    error_messages = []
    parser = argparse.ArgumentParser(description='Download dependent tool packages.')
    parser.add_argument('-d', '--project', help='project root directory')
    parser.add_argument('product', help='selected project')

    args = parser.parse_args()
    cache_file = os.path.join(args.project if args.project else os.getcwd(),
        '.deveco', 'status', f'{args.product.replace("@", ".")}')

    if not os.path.isfile(cache_file):
        print(f'Failed to download dependent tools for product: {args.product}.')
        print(f'Can not find status result file: {cache_file}')
        return

    pid_file = os.path.join(os.path.dirname(cache_file), 'pid')
    with os.fdopen(os.open(pid_file, os.O_RDWR | os.O_CREAT | os.O_TRUNC, 0o640), 'w') as setting_file:
        setting_file.write(str(os.getpid()))

    tool_status = json.load(open(cache_file, 'r'))
    if tool_status.get('all_valid'):
        print(f'Nothing to do because all dependent tools are valid for product: {args.product}. Quit.')
        return

    # download deb packages
    deb_tools = tool_status.get('tools', {}).get('deb', {})
    deb_invalid_tools = [
        f'{tool}{deb_tools[tool].get("version", "")}' \
            for tool in deb_tools if not deb_tools[tool].get('valid')
    ]
    for tool in deb_invalid_tools:
        print('='*50 + '\n')
        print(f'Download deb package: {tool}\n')
        print('-'*50)
        cp = subprocess.run(['sudo', 'apt', 'install', tool, '-y'])
        print(f'exit code: {cp.returncode}')
        print('-'*50)

    # download pip packages
    pip_tools = tool_status.get('tools', {}).get('pip', {})
    pip_invalid_tools = [
        f'{tool}{pip_tools[tool].get("version", "")}' \
            for tool in pip_tools if not pip_tools[tool].get('valid')
    ]
    for tool in pip_invalid_tools:
        print('='*50 + '\n')
        print(f'Download pip package: {tool}\n')
        print('-'*50)
        cp = subprocess.run([sys.executable, '-m', 'pip', 'install', '--user', tool])
        print(f'exit code: {cp.returncode}')
        print('-'*50)

    # download prebuilt packages
    prebuilt_tools = tool_status.get('tools', {}).get('prebuilt', {})
    prebuilt_invalid_tools = []
    for tool in prebuilt_tools:
        if not prebuilt_tools[tool].get('valid') and \
        prebuilt_tools[tool].get('store'):
            prebuilt_tools[tool]["name"] = tool
            prebuilt_invalid_tools.append(prebuilt_tools[tool])
            
    compiler_tools = tool_status.get('tools', {}).get('compiler', {})
    compiler_invalid_tools = []
    for tool in compiler_tools:
        if not compiler_tools[tool].get('valid') and \
        not compiler_tools[tool].get('custom') and \
        compiler_tools[tool].get('store'):
            compiler_tools[tool]["name"] = tool
            compiler_invalid_tools.append(compiler_tools[tool])


    pre_invalid_tools = prebuilt_invalid_tools + compiler_invalid_tools
    if pre_invalid_tools:
        cp = subprocess.run([sys.executable, '-m', 'pip', 'show', 'requests'],
            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, timeout=10)
        if cp.returncode != 0:
            subprocess.run([sys.executable, '-m', 'pip', 'install', '--user', 'requests'],
                stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, timeout=10)

    for tool in pre_invalid_tools:
        if not tool.get('url'):
            error_messages.append(
                f"\033[1;31m[Error] Failed to download {tool.get('name', 'tool')}. Please manually download and configure the path. \033[0m")
            continue
        print('='*50 + '\n')
        storage_path = tool.get('store')
        dest_file = os.path.join(storage_path, os.path.basename(tool.get('url')))
        dest_sha_file = dest_file + '.sha256'
        if not os.path.isdir(storage_path):
            os.makedirs(storage_path)
        try:
            download_tool(tool.get('url'), dest_file)
            download_tool(tool.get('url') + '.sha256', dest_sha_file)
            res = check_file_sha256(dest_file, dest_sha_file)
            print(f'Check Sha256 Ok?  {"Yes" if res else "No"} \n')
            print(f"Install: {tool.get('install_command')}")
            cp = subprocess.run(tool.get('install_command'), cwd=storage_path, shell=True)
            print(f'exit code: {cp.returncode}')
        except Exception as e:
            print(e)
        if os.path.isfile(dest_file):
            os.remove(dest_file)
        if os.path.isfile(dest_sha_file):
            os.remove(dest_sha_file)
        print('-'*50)

    for message in error_messages:
        print(message)
    print(f'\n\nIf you have network problems, you can refer to link: \n{HELP_LINK}\n\n')


if __name__ == '__main__':
    main()
