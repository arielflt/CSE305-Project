# CSE305-Project: Andrea Foffani Pifarré, Ariel Flahaut & Arjun Bommadevara

If the code is run from ssh, it requires to add the path to Magick++ every time. This can be done by using this:

export PATH=$HOME/ImageMagick/bin:$PATH
export LD_LIBRARY_PATH=$HOME/ImageMagick/lib:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=$HOME/ImageMagick/lib/pkgconfig:$PKG_CONFIG_PATH 

If the user is not using ssh, it may still be necessary to add some of the flags below. To run the basic algorithm implementation this code can be used:

g++ -o nbody_simulation nbody_simulation.cpp -I/$HOME/ImageMagick/include/ImageMagick-7 -L/$HOME/ImageMagick/lib -lMagick++-7.Q16HDRI -lMagickWand-7.Q16HDRI -lMagickCore-7.Q16HDRI -std=c++11 -lpthread -DMAGICKCORE_QUANTUM_DEPTH=16 -DMAGICKCORE_HDRI_ENABLE=1

This is the code for the sequential Barnes-Hut algorithm:

g++ -std=c++11 -fopenmp -o nbody_simulation2 nbody_simulation2.cpp barnes_hut.cpp -I/$HOME/ImageMagick/include/ImageMagick-7 -L/$HOME/ImageMagick/lib -lMagick++-7.Q16HDRI -lMagickWand-7.Q16HDRI -lMagickCore-7.Q16HDRI -lpthread -DMAGICKCORE_QUANTUM_DEPTH=16 -DMAGICKCORE_HDRI_ENABLE=1

And finally for the parallelised Barnes-Hut algorithm:

g++ -std=c++11 -fopenmp -o nbody_simulation_bhmulti nbody_simulation_bhmulti.cpp barnes_hut_multi.cpp -I/$HOME/ImageMagick/include/ImageMagick-7 -L/$HOME/ImageMagick/lib -lMagick++-7.Q16HDRI -lMagickWand-7.Q16HDRI -lMagickCore-7.Q16HDRI -lpthread -DMAGICKCORE_QUANTUM_DEPTH=16 -DMAGICKCORE_HDRI_ENABLE=1


Note that even if the code is not run through ssh, the following flags will still be necessary: -std=c++11 -lpthread -DMAGICKCORE_QUANTUM_DEPTH=16 -DMAGICKCORE_HDRI_ENABLE=1
