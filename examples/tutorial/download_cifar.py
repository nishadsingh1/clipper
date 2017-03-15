import urllib
import sys
import os

def download_cifar(loc):
    # download and extract the data
    if not os.path.exists(loc):
        os.makedirs(loc)
    if not os.path.exists(os.path.join(loc, 'cifar-10-python.tar.gz')):
        print("CIFAR10 dataset not found, downloading...")
        # http://stackoverflow.com/a/19602990
        testfile = urllib.URLopener()
        testfile.retrieve(
            'https://www.cs.toronto.edu/~kriz/cifar-10-python.tar.gz',
            os.path.join(loc, 'cifar-10-python.tar.gz'))
        print("Finished downloading")
    else:
        print("CIFAR10 dataset found! If you wish to redownload, delete cifar-10-python.tar.gz at the given download location and re-run this command.")

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: ./download_cifar.py <download_path>")
        sys.exit(1)
    download_cifar(sys.argv[1])
