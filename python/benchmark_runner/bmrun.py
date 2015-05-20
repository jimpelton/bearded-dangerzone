__author__ = 'Jim'

import sys
import os
import io
import collections


class MajorError(Exception):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)


class TestParams():
    def __init__(self):
        # All your args are belong to this map!
        self.cl_args = collections.OrderedDict()
        self.cl_args['file'] = 'volume.raw'
        self.cl_args['tfunc'] = 'default.1dt'
        self.cl_args['camera-pos'] = 0
        self.cl_args['num-slices'] = 64
        self.cl_args['nbx'] = 1
        self.cl_args['nby'] = 1
        self.cl_args['nbz'] = 1
        self.cl_args['volx'] = 1
        self.cl_args['voly'] = 1
        self.cl_args['volz'] = 1
        self.cl_args['type'] = 'float'
        self.cl_args['tmax'] = 1.0
        self.cl_args['tmin'] = 0.0
        self.cl_args['perf-out-file'] = 'counters.txt'
        self.cl_args['perf-mode'] = ''
        # self.cl_args['perf-dll-path'] = 'virus.dll'

        # misc vals
        self.exe_path = "I don't exist.exe"
        self.gpu_model = '780ti'
        self.wat = 'sphere'
        self.desc = 'description'
        self.output_directory_path = ''

    def _test_int(self, i):
        try:
            return int(i)
        except:
            raise MajorError("amount warn't an int-like.")

    def increment_num_blocks(self, amount):
        a = self._test_int(amount)
        nbx = self.cl_args["nbx"] + a
        nby = self.cl_args["nby"] + a
        nbz = self.cl_args["nbz"] + a
        self.cl_args["nbx"] = nbx
        self.cl_args["nby"] = nby
        self.cl_args["nbz"] = nbz
        return

    def increment_num_slices(self, amount):
        a = self._test_int(amount)
        ns = self.cl_args["num-slices"] + a
        self.cl_args["num-slices"] = ns
        return

    def set_raw_filepath(self, path):
        self.cl_args['file'] = os.path.normpath(path)
        return

    def set_tfunc_filepath(self, path):
        self.cl_args['tfunc'] = os.path.normpath(path)
        return

    def set_camera_pos(self, npos):
        p = self._test_int(npos)
        self.cl_args['camera-pos'] = p
        return

    def set_num_slices(self, nSlices):
        n = self._test_int(nSlices)
        self.cl_args['num-slices'] = n
        return

    def set_vol_dims(self, xdim, ydim, zdim):
        x, y, z = self._test_int(xdim), self._test_int(ydim), self._test_int(zdim)
        self.cl_args['volx'] = x
        self.cl_args['voly'] = y
        self.cl_args['volz'] = z
#        self.cl_args['perf-out-file'] = os.path.join(self.get_output_directory_path(), self.get_output_file_string())
        return

    def set_type(self, type):
        self.cl_args['type'] = type
        return

    def set_tmax(self, max):
        self.cl_args['tmax'] = float(max)
        return

    def set_tmin(self, min):
        self.cl_args['tmin'] = float(min)
        return

    def set_perf_out_file_name(self, outfile):
        self.cl_args['perf-out-file'] = os.path.join(self.get_output_directory_path(), os.path.normpath(outfile))
        return

    def set_exe_path(self, exepath):
        self.exe_path = os.path.normpath(exepath)
        return

    # def set_perf_dll_path(self, dllpath):
    #     self.cl_args['perf-dll-path'] = os.path.normpath(dllpath)
    #     return

    def get_perf_file_string(self):
        return "{gpu}_{wat}_{vx}-{vy}-{vz}_{nx}-{ny}-{nz}_s{ns}_c{cp}.txt" \
            .format(gpu=self.gpu_model, wat=self.wat,
                    vx=self.cl_args['volx'], vy=self.cl_args['voly'], vz=self.cl_args['volz'],
                    nx=self.cl_args['nbx'], ny=self.cl_args['nby'], nz=self.cl_args['nbz'],
                    ns=self.cl_args['num-slices'], cp=self.cl_args['camera-pos'])

    def get_output_directory_name(self):
        return "{gpu}_{wat}_{vx}-{vy}-{vz}_{nx}-{ny}-{nz}_s{ns}" \
            .format(gpu=self.gpu_model, wat=self.wat,
                    vx=self.cl_args['volx'], vy=self.cl_args['voly'], vz=self.cl_args['volz'],
                    nx=self.cl_args['nbx'], ny=self.cl_args['nby'], nz=self.cl_args['nbz'],
                    ns=self.cl_args['num-slices'])

    def get_output_directory_path(self):
        # return os.path.dirname(self.cl_args['perf-out-file'])
        return self.output_directory_path

    def set_output_directory_path(self, path):
        self.output_directory_path = os.path.normpath(path)
        # update the cl_args output file with new leading path info.
        current_fname = self.cl_args['perf-out-file']
        idx = current_fname.rfind(os.path.pathsep)
        if idx > -1:
            perf_name = current_fname[current_fname.rindex(os.path.pathsep)]
        else:
            perf_name = current_fname
        self.cl_args['perf-out-file'] = os.path.join(self.output_directory_path, perf_name)
        return

    def build_command_line(self):
        self.cl_args['perf-out-file'] = os.path.join(self.output_directory_path, self.get_perf_file_string())
        # cl = io.StringIO()
        cl = []
        for k, v in self.cl_args.items():
            cl.append('--{key}'.format(key=k))
            cl.append('{val}'.format(val=str(v)))
            # cl.write("--{arg_name} {arg_val} ".format(arg_name=str(k), arg_val=str(v)))
        # cl.seek(0)
        # return cl.readline()
        return cl

    def run_test(self):
        pass


####################################################################################################################
def check_for_and_make_outut_dir(path):
    succeeeeedy = True
    # Check that dir for perf-out-file exists.
    if not os.path.exists(path):
        os.mkdir(path)
        if not os.path.exists(path):
            succeeeeedy = False
    return succeeeeedy


# TODO: run_tests takes min/max loop invariants
def run_tests(params):
    for i in range(0, 16):
        test_name = params.get_output_directory_name()
        print("Starting test: {}".format(test_name))
        if not check_for_and_make_outut_dir(params.get_output_directory_path()):
            print("Could not make directory for perf out file: {}".format(test_name))
        print("Made directory {}".format(test_name))

        for c in [0, 1, 2]:
            params.set_camera_pos(c)
            command_line = params.build_command_line()
            print("Command line {} {}".format(params.exe_path, command_line))
            # run the command!
            import subprocess
            cl = [params.exe_path]
            for x in command_line:
                cl.append(x)

            subprocess.call(cl, cwd=os.path.dirname(params.exe_path))
            from time import sleep
            sleep(2)

        params.increment_num_blocks(1)
        numblocks = params.cl_args['nbx']
        params.set_num_slices(64 / numblocks)

    return


def main(argv):
    if len(argv) < 4:
        print("Usage: <exe-path> <raw-file> <tfunc> [output-dir]")
        exit(1)



    # Intitial test parameters
    params = TestParams()
    params.set_exe_path(argv[1])
    # params.set_exe_path("C:/Users/Jim/Documents/programming/thesis/bearded-dangerzone.git/build/"
    # "simple_blocks/Release/simple_blocks.exe")
    params.set_raw_filepath(argv[2])
    # params.set_raw_filepath("D:/volumes/big_sphere_1k/sphere_1000x1000x1000.raw")
    params.set_tfunc_filepath(argv[3])
    # params.set_tfunc_filepath("D:/volumes/big_sphere_1k/default.1dt")
    params.set_output_directory_path(argv[4])
    # params.set_perf_dll_path(argv[5])
    # params.set_perf_out_file_name(os.path.join("D:/perfout/", params.get_output_file_string()))
    params.wat = 'sphere'
    params.set_type('float')
    params.set_vol_dims(1000, 1000, 1000)
    params.set_num_slices(64)

    # Check that exe, raw and transfer function files exist.
    nope = False
    if not os.path.exists(params.exe_path):
        print("{} does not exist.".format(params.exe_path))
        nope = True
    if not os.path.exists(params.cl_args['file']):
        print("{} does not exist.".format(params.cl_args['file']))
        nope = True
    if not os.path.exists(params.cl_args['tfunc']):
        print("{} does not exist.".format(params.cl_args['tfunc']))
        nope = True

    if nope:
        print("Error! Error! Eee! Exiting...")
        exit(1)

    run_tests(params)

    return


if __name__ == "__main__":
    main(sys.argv)
