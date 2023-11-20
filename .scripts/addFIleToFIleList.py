import json
import os.path
import sys
import glob

def verfyFiles(files):
    '''
    update the file list to containt a sorted by index list of all the c* files in src

    files: array of [filename, fileid] of the old files
    return: the updated file array
    '''
    files = [file for file in files if os.path.isfile(file[0])]
    files = sorted(files, key=lambda x: x[1])
    for filename in list(glob.iglob('src/**/*.c*', recursive=True)) + list(glob.iglob('tests/**/**.c*', recursive=True)):
        try:
            fileIndex = [file[0] for file in files].index(filename)
        except ValueError:
            files.append((filename, find_smallest_available_id(files)))
    return files



def find_smallest_available_id(files):
    """
    return the smallest missing id in files(if there is 1,2, 4 it will return 3)
    assume files is orderd by ids
    """
    j = 0
    for file in files:
        if file[1] > j:
            return j
        j += 1
    return j

def main():
    with open(".build/include/files.json", "a+") as f:
        f.seek(0)
        files = []
        try:
            files = json.load(f)
        except json.decoder.JSONDecodeError:
            pass
        files = verfyFiles(files)
        f.truncate(0)
        f.seek(0)
        json.dump(files, f)

if __name__ == "__main__":
    main()
