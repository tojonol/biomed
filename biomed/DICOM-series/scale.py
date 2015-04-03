#!/usr/bin/env python

import os
import os.path
import sys

if __name__ == '__main__':
    source_dir = sys.argv[1]
    dest_dir = source_dir + '_SCALED'

    os.makedirs(dest_dir)

    if len(sys.argv) > 2:
        scale_factor = 1.0 / float(sys.argv[2])
    else:
        scale_factor = 0.25

    files = os.listdir(source_dir)

    for idx, f in enumerate(files):
        cmd = 'dcmscale +Sxf %f %s %s' % (
            scale_factor,
            os.path.join(source_dir, f),
            os.path.join(dest_dir, f))

        sys.stdout.write('Processing %d of %d...' % (idx+1, len(files)))
        os.system(cmd)
        sys.stdout.write(' done!\n')
