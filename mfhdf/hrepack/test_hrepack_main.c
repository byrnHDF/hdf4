/****************************************************************************
 * NCSA HDF                                                                 *
 * Software Development Group                                               *
 * National Center for Supercomputing Applications                          *
 * University of Illinois at Urbana-Champaign                               *
 * 605 E. Springfield, Champaign IL 61820                                   *
 *                                                                          *
 * For conditions of distribution and use, see the accompanying             *
 * hdf/COPYING file.                                                        *
 *                                                                          *
 ****************************************************************************/


#include "hdf.h"
#include "mfhdf.h"
#include "hcomp.h"
#include "hrepack.h"
#include "hdiff.h"
#include "test_hrepack_add.h"
#include "test_hrepack_verify.h"

#define DATA_FILE1       "image8.txt"
#define DATA_FILE2       "image24pixel.txt"
#define DATA_FILE3       "image24plane.txt"
char    *progname;   

/* for old branch compability */
#if 0
#define H4_HAVE_LIBSZ
#endif
  


/*-------------------------------------------------------------------------
 * Function: main
 *
 * Purpose: test program for hrepack
 *
 * A)This program writes several HDF objects to the file FILENAME
 *   The image data consists of realistic data read from the files DATA_FILE1
 *   (8bit image) , DATA_FILE2 (24bit image, pixel interlace) and
 *    DATA_FILE3 (24bit image, plane interlace)
 *  The objects written are
 *  1) groups
 *  2) images 
 *  3) datasets
 *  4) vdatas with attributes and field attributes
 *  5) global and local attributes
 *  6) labels and annotations
 *
 * B) Then several calls are made to hrepack, in each call the FILENAME_OUT is 
 *  generated
 *
 * C) In each test the hdiff utility is called to compare the files 
 *  FILENAME and FILENAME_OUT
 *
 * D) In each test the verifiy_comp_chunk function is called to compare 
 *  the input and output compression and chunking parameters
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *
 * Date: August 3, 2003
 *
 *-------------------------------------------------------------------------
 */

int main(void)
{
 int32         vgroup1_id,   /* vgroup identifier */
               vgroup2_id,   /* vgroup identifier */
               vgroup3_id,   /* vgroup identifier */
               vgroup_img_id,/* vgroup identifier */
               file_id;      /* HDF file identifier, same for V interface */
 options_t     options;      /* for hrepack  */ 
 static diff_opt_t fspec;  /* for hdiff  */ 
 int           verbose=0;
 int32         attr_n_values = 3;  /* number of values in the vg attribute */
 char          vg_attr[3]    = {'A', 'B', 'C'};/* vg attribute values*/
 comp_coder_t  comp_type;    /* to retrieve compression type into */
 int32         chunk_flags;  /* Chunking flag */ 
 comp_info     comp_info;    /* compression structure */ 
 int32         in_chunk_lengths[MAX_VAR_DIMS];

 /* initialize options for hdiff */
 memset(&fspec,0,sizeof(diff_opt_t));
 fspec.ga = 1;    /* compare global attributes */
 fspec.sa = 1;    /* compare SD local attributes */
 fspec.sd = 1;    /* compare SD data */
 fspec.vd = 1;    /* compare Vdata */
 fspec.gr = 1;    /* compare GR data */

/*-------------------------------------------------------------------------
 * create a file with SDSs, images , groups and vdatas
 *-------------------------------------------------------------------------
 */
       
 /* create a HDF file */
 file_id = Hopen (FILENAME, DFACC_CREATE, 0);
  
 /* initialize the V interface */
 if (Vstart (file_id)==FAIL){
  printf( "Could not start VG\n");
 }


/*-------------------------------------------------------------------------
 * start
 *-------------------------------------------------------------------------
 */ 

 /* create the first vgroup.  the vgroup reference number is set
  * to -1 for creating and the access mode is "w" for writing
  */
 vgroup1_id = Vattach (file_id, -1, "w");
 if (Vsetname (vgroup1_id, "g1")==FAIL){
  printf( "Could not name group\n");
 }

 /* attach an attribute to the vgroup */
 if (Vsetattr (vgroup1_id,"Myattr",DFNT_CHAR,attr_n_values,vg_attr)==FAIL){
  printf( "Could set group attributes\n");
 }
 
 /* create the second vgroup */
 vgroup2_id = Vattach (file_id, -1, "w");
 if (Vsetname (vgroup2_id, "g2")==FAIL){
  printf( "Could not name group\n");
 }

 /* create the 3rd vgroup */
 vgroup3_id = Vattach (file_id, -1, "w");
 if (Vsetname (vgroup3_id, "g3")==FAIL){
  printf( "Could not name group\n");
 }

 /* insert the second vgroup into the first vgroup using its identifier */
 if (Vinsert (vgroup1_id, vgroup2_id)==FAIL){
  printf( "Could not insert VG\n");
 }

 /* insert the 3rd vgroup into the 2nd vgroup using its identifier */
 if (Vinsert (vgroup2_id, vgroup3_id)==FAIL){
  printf( "Could not insert VG\n");
 }

 /* create the 4th vgroup, for images */
 vgroup_img_id = Vattach (file_id, -1, "w");
 if (Vsetname (vgroup_img_id, "images")==FAIL){
  printf( "Could not name group\n");
 }
 
/*-------------------------------------------------------------------------
 * add some SDS to the file
 * duplicates are inserted in the groups "g1", "g2", "g3" and root
 *-------------------------------------------------------------------------
 */ 

 /* add non chunked, non compressed sds */
 chunk_flags = HDF_NONE;
 comp_type   = COMP_CODE_NONE;
 add_sd(FILENAME,file_id,"dset1",vgroup1_id,chunk_flags,comp_type,NULL);
 add_sd(FILENAME,file_id,"dset2",vgroup2_id,chunk_flags,comp_type,NULL);
 add_sd(FILENAME,file_id,"dset3",vgroup3_id,chunk_flags,comp_type,NULL);
 add_sd(FILENAME,file_id,"dset4",0,chunk_flags,comp_type,NULL);
 add_sd(FILENAME,file_id,"dset5",0,chunk_flags,comp_type,NULL);
 add_sd(FILENAME,file_id,"dset6",0,chunk_flags,comp_type,NULL);
 add_sd3d(FILENAME,file_id,"dset7",0,chunk_flags,comp_type,NULL);


/*-------------------------------------------------------------------------
 * add some chunked/compressd SDS to the file
 * Chunked                  -> flags = HDF_CHUNK
 * Chunked and compressed   -> flags = HDF_CHUNK | HDF_COMP 
 * Non-chunked              -> flags = HDF_NONE
 *-------------------------------------------------------------------------
 */ 

 /* add a chunked, non compressed sds */
 chunk_flags = HDF_CHUNK;
 comp_type   = COMP_CODE_NONE;
 add_sd(FILENAME,file_id,"dset_chunk",0,chunk_flags,comp_type,NULL);

 /* add a chunked-compressed sds with SDsetchunk */
 chunk_flags = HDF_CHUNK | HDF_COMP;
 comp_type   = COMP_CODE_DEFLATE;
 add_sd(FILENAME,file_id,"dset_chunk_comp",0,chunk_flags,comp_type,&comp_info);

/*-------------------------------------------------------------------------
 * GZIP
 *-------------------------------------------------------------------------
 */ 

 /* add some non chunked, compressed sds */
 chunk_flags = HDF_NONE;
 comp_type   = COMP_CODE_DEFLATE;
 add_sd(FILENAME,file_id,"dset_gzip",0,chunk_flags,comp_type,&comp_info);

/*-------------------------------------------------------------------------
 * RLE
 *-------------------------------------------------------------------------
 */ 

 /* add some non chunked, compressed sds */
 chunk_flags = HDF_NONE;
 comp_type   = COMP_CODE_RLE;
 add_sd(FILENAME,file_id,"dset_rle",0,chunk_flags,comp_type,&comp_info);

/*-------------------------------------------------------------------------
 * HUFF
 *-------------------------------------------------------------------------
 */ 

 /* add some non chunked, compressed sds */
 chunk_flags = HDF_NONE;
 comp_type   = COMP_CODE_SKPHUFF;
 add_sd(FILENAME,file_id,"dset_huff",0,chunk_flags,comp_type,&comp_info);

#if defined (H4_HAVE_LIBSZ)
/*-------------------------------------------------------------------------
 * SZIP
 *-------------------------------------------------------------------------
 */ 
 chunk_flags = HDF_NONE;
 comp_type   = COMP_CODE_SZIP;
 add_sd(FILENAME,file_id,"dset_szip",0,chunk_flags,comp_type,&comp_info);
  
 add_sd_szip_all(FILENAME,file_id,0);
 
#endif

/*-------------------------------------------------------------------------
 * add some RIS24 images to the file
 *-------------------------------------------------------------------------
 */
 add_r24(DATA_FILE2,FILENAME,file_id,DFIL_PIXEL,vgroup_img_id); /* Pixel Interlacing */
 add_r24(DATA_FILE3,FILENAME,file_id,DFIL_PLANE,vgroup_img_id); /* Scan Plane Interlacing */


/*-------------------------------------------------------------------------
 * add some RIS8 images to the file
 *-------------------------------------------------------------------------
 */ 
 add_r8(DATA_FILE1,FILENAME,file_id,vgroup_img_id);

/*-------------------------------------------------------------------------
 * add some GR images to the file with compression/chunking
 *-------------------------------------------------------------------------
 */ 

/*-------------------------------------------------------------------------
 * no compression
 *-------------------------------------------------------------------------
 */ 

 chunk_flags = HDF_NONE;
 comp_type   = COMP_CODE_NONE;
 add_gr("gr_none",file_id,0,chunk_flags,comp_type,&comp_info);


/*-------------------------------------------------------------------------
 * GZIP
 *-------------------------------------------------------------------------
 */ 

 chunk_flags = HDF_NONE;
 comp_type   = COMP_CODE_DEFLATE;
 add_gr("gr_gzip",file_id,0,chunk_flags,comp_type,&comp_info);

/*-------------------------------------------------------------------------
 * SZIP
 *-------------------------------------------------------------------------
 */ 

#if defined (H4_HAVE_LIBSZ)

 chunk_flags = HDF_NONE;
 comp_type   = COMP_CODE_SZIP;
 add_gr("gr_szip",file_id,0,chunk_flags,comp_type,&comp_info);

#endif

/*-------------------------------------------------------------------------
 * add some GR realistic images to the file
 * realistic data is read from ASCII files
 *-------------------------------------------------------------------------
 */ 

 add_gr_ffile(DATA_FILE1,"gr_8bit",0,file_id,0);
 add_gr_ffile(DATA_FILE2,"gr_24bit",0,file_id,0);


/*-------------------------------------------------------------------------
 * add some VS to the file
 * duplicates are inserted in the groups "g1", "g2", "g3" and root
 *-------------------------------------------------------------------------
 */ 

 add_vs("vdata1",file_id,vgroup1_id);
 add_vs("vdata2",file_id,vgroup2_id);
 add_vs("vdata3",file_id,vgroup3_id);
 add_vs("vdata4",file_id,0);

/*-------------------------------------------------------------------------
 * add some global attributes to the file
 *-------------------------------------------------------------------------
 */ 
 add_glb_attrs(FILENAME,file_id);

/*-------------------------------------------------------------------------
 * add annotations to the file
 *-------------------------------------------------------------------------
 */ 
 add_file_an(file_id);

/*-------------------------------------------------------------------------
 * add a palette to the file
 *-------------------------------------------------------------------------
 */ 
 add_pal(FILENAME);

/*-------------------------------------------------------------------------
 * end
 *-------------------------------------------------------------------------
 */

 /* terminate access to the vgroups */
 if (Vdetach (vgroup1_id)==FAIL ||
     Vdetach (vgroup2_id)==FAIL ||
     Vdetach (vgroup3_id)==FAIL ||
     Vdetach (vgroup_img_id)==FAIL){
  printf( "Could not close group\n");
 }
 
 /* terminate access to the V interface */
 if (Vend (file_id)==FAIL){
  printf( "Could not end VG\n");
 }

 /* close the HDF file */
 if (Hclose (file_id)==FAIL){
  printf( "Could not close file\n");
 }

/*-------------------------------------------------------------------------
 * TESTS:
 * 1) zip FILENAME with some compression/chunking options
 * 2) use the hdiff utility to compare the input and output file; it returns RET==0
 *    if the high-level objects have the same data
 * 3) use the API functions SD(GR)getcompress, SD(GR)getchunk to verify
 *    the compression/chunking input on the otput file
 *-------------------------------------------------------------------------
 */

#if defined (HZIPTST_DEBUG)
 verbose        =1;
 fspec.verbose  =1;
#endif

 in_chunk_lengths[0]=10;
 in_chunk_lengths[1]=8;
 in_chunk_lengths[2]=6;


/*-------------------------------------------------------------------------
 * test0:  
 *-------------------------------------------------------------------------
 */
 TESTING("copying file with no options");
 hrepack_init (&options,verbose);
 hrepack(FILENAME,FILENAME_OUT,&options);
 hrepack_end (&options);
 if (hdiff(FILENAME,FILENAME_OUT,&fspec) == 1)
  goto out;
 PASSED();


/*-------------------------------------------------------------------------
 * test1:  
 *-------------------------------------------------------------------------
 */
 TESTING("compressing SDS SELECTED with HUFF, chunking SELECTED");
 hrepack_init (&options,verbose);
 hrepack_addcomp("dset7:HUFF 1",&options);
 hrepack_addchunk("dset7:10x8x6",&options);
 hrepack(FILENAME,FILENAME_OUT,&options);
 hrepack_end (&options);
 if (hdiff(FILENAME,FILENAME_OUT,&fspec) == 1)
  goto out;
 if ( sds_verifiy_comp("dset7",COMP_CODE_SKPHUFF, 1) == -1) 
  goto out;
 if ( sds_verifiy_chunk("dset7",HDF_CHUNK|HDF_COMP,3,in_chunk_lengths) == -1) 
  goto out;
 PASSED();


/*-------------------------------------------------------------------------
 * test2:  
 *-------------------------------------------------------------------------
 */
 TESTING("compressing SDS SELECTED with RLE, chunking SELECTED");
 hrepack_init (&options,verbose);
 hrepack_addcomp("dset4:RLE",&options);
 hrepack_addchunk("dset4:10x8",&options);
 hrepack(FILENAME,FILENAME_OUT,&options);
 hrepack_end (&options);
 if (hdiff(FILENAME,FILENAME_OUT,&fspec) == 1)
  goto out;
 if ( sds_verifiy_comp("dset4",COMP_CODE_RLE, 0) == -1) 
  goto out;
 if ( sds_verifiy_chunk("dset4",HDF_CHUNK|HDF_COMP,2,in_chunk_lengths) == -1) 
  goto out;
 PASSED();

/*-------------------------------------------------------------------------
 * test3:  
 *-------------------------------------------------------------------------
 */
 TESTING("compressing SDS SELECTED with GZIP, chunking SELECTED");
 hrepack_init (&options,verbose);
 hrepack_addcomp("dset4:GZIP 6",&options);
 hrepack_addchunk("dset4:10x8",&options);
 hrepack(FILENAME,FILENAME_OUT,&options);
 hrepack_end (&options);
 if (hdiff(FILENAME,FILENAME_OUT,&fspec) == 1)
  goto out;
 if ( sds_verifiy_comp("dset4",COMP_CODE_DEFLATE, 6) == -1) 
  goto out;
 if ( sds_verifiy_chunk("dset4",HDF_CHUNK|HDF_COMP,2,in_chunk_lengths) == -1) 
  goto out;
 PASSED();

#if defined (H4_HAVE_LIBSZ)

/*-------------------------------------------------------------------------
 * test4:  
 *-------------------------------------------------------------------------
 */
 TESTING("compressing SDS SELECTED with SZIP, chunking SELECTED");
 hrepack_init (&options,verbose);
 hrepack_addcomp("dset4:SZIP",&options);
 hrepack_addchunk("dset4:10x8",&options);
 hrepack(FILENAME,FILENAME_OUT,&options);
 hrepack_end (&options);
 if (hdiff(FILENAME,FILENAME_OUT,&fspec) == 1)
  goto out;
 if ( sds_verifiy_comp("dset4",COMP_CODE_SZIP, 0) == -1) 
  goto out;
 if ( sds_verifiy_chunk("dset4",HDF_CHUNK|HDF_COMP,2,in_chunk_lengths) == -1) 
  goto out;
 PASSED();

#endif

/*-------------------------------------------------------------------------
 * test4:  
 *-------------------------------------------------------------------------
 */
 TESTING("compressing SDS SELECTED with NONE, chunking SELECTED NONE");
 hrepack_init (&options,verbose);
 hrepack_addcomp("dset_chunk_comp:NONE",&options);
 hrepack_addcomp("dset_chunk:NONE",&options);
 hrepack_addchunk("dset_chunk_comp:NONE",&options);
 hrepack_addchunk("dset_chunk:NONE",&options);
 hrepack(FILENAME,FILENAME_OUT,&options);
 hrepack_end (&options);
 if (hdiff(FILENAME,FILENAME_OUT,&fspec) == 1)
  goto out;
 if ( sds_verifiy_comp("dset_chunk_comp",COMP_CODE_NONE, 0) == -1) 
  goto out;
 if ( sds_verifiy_comp("dset_chunk",COMP_CODE_NONE, 0) == -1) 
  goto out;
 if ( sds_verifiy_chunk("dset_chunk_comp",HDF_NONE,0,0) == -1) 
  goto out;
 if ( sds_verifiy_chunk("dset_chunk",HDF_NONE,0,0) == -1) 
  goto out;
 PASSED();

#if defined (H4_HAVE_LIBSZ)

/*-------------------------------------------------------------------------
 * test5:  
 *-------------------------------------------------------------------------
 */
 TESTING("compressing SDS SELECTED with all types, chunking SELECTED");
 hrepack_init (&options,verbose);
 hrepack_addcomp("dset4:GZIP 9",&options);
 hrepack_addcomp("dset5:RLE",&options);
 hrepack_addcomp("dset6:HUFF 2",&options);
 hrepack_addcomp("dset7:SZIP",&options);
 hrepack_addchunk("dset4:10x8",&options);
 hrepack_addchunk("dset5:10x8",&options);
 hrepack_addchunk("dset6:10x8",&options);
 hrepack(FILENAME,FILENAME_OUT,&options);
 hrepack_end (&options);
 if (hdiff(FILENAME,FILENAME_OUT,&fspec) == 1)
  goto out;
 if ( sds_verifiy_comp("dset4",COMP_CODE_DEFLATE, 9) == -1) 
  goto out;
 if ( sds_verifiy_comp("dset5",COMP_CODE_RLE, 0) == -1) 
  goto out;
 if ( sds_verifiy_comp("dset6",COMP_CODE_SKPHUFF, 2) == -1) 
  goto out;
 if ( sds_verifiy_comp("dset7",COMP_CODE_SZIP, 0) == -1) 
  goto out;
 if ( sds_verifiy_chunk("dset4",HDF_CHUNK|HDF_COMP,2,in_chunk_lengths) == -1) 
  goto out;
 if ( sds_verifiy_chunk("dset5",HDF_CHUNK|HDF_COMP,2,in_chunk_lengths) == -1) 
  goto out;
 if ( sds_verifiy_chunk("dset6",HDF_CHUNK|HDF_COMP,2,in_chunk_lengths) == -1) 
  goto out;
 PASSED();


/*-------------------------------------------------------------------------
 * test6:  
 *-------------------------------------------------------------------------
 */
 TESTING("compressing SDS SELECTED with all types, no chunking");
 hrepack_init (&options,verbose);
 hrepack_addcomp("dset4:GZIP 9",&options);
 hrepack_addcomp("dset5:RLE",&options);
 hrepack_addcomp("dset6:HUFF 2",&options);
 hrepack_addcomp("dset7:SZIP",&options);
 hrepack(FILENAME,FILENAME_OUT,&options);
 hrepack_end (&options);
 if (hdiff(FILENAME,FILENAME_OUT,&fspec) == 1)
  goto out;
 if ( sds_verifiy_comp("dset4",COMP_CODE_DEFLATE, 9) == -1) 
  goto out;
 if ( sds_verifiy_comp("dset5",COMP_CODE_RLE, 0) == -1) 
  goto out;
 if ( sds_verifiy_comp("dset6",COMP_CODE_SKPHUFF, 2) == -1) 
  goto out;
 if ( sds_verifiy_comp("dset7",COMP_CODE_SZIP, 0) == -1) 
  goto out;
 PASSED();

#endif


/*-------------------------------------------------------------------------
 * test7:  
 *-------------------------------------------------------------------------
 */

 TESTING("compressing SDS ALL, chunking SELECTED NONE");
 hrepack_init (&options,verbose);
 hrepack_addcomp("*:GZIP 1",&options);
 hrepack_addchunk("dset_chunk_comp:NONE",&options);
 hrepack_addchunk("dset_chunk:NONE",&options);
 hrepack(FILENAME,FILENAME_OUT,&options);
 hrepack_end (&options);
 if (hdiff(FILENAME,FILENAME_OUT,&fspec) == 1)
  goto out;
 if ( sds_verifiy_comp_all(COMP_CODE_DEFLATE, 1) == -1) 
  goto out;
 if ( sds_verifiy_chunk("dset_chunk_comp",HDF_NONE,0,0) == -1) 
  goto out;
 if ( sds_verifiy_chunk("dset_chunk",HDF_NONE,0,0) == -1) 
  goto out;
 PASSED();

/*-------------------------------------------------------------------------
 * test8:  
 *-------------------------------------------------------------------------
 */

 TESTING("no compressing, chunking ALL");
 hrepack_init (&options,verbose);
 hrepack_addchunk("*:10x8",&options);
 hrepack(FILENAME,FILENAME_OUT,&options);
 hrepack_end (&options);
 if (hdiff(FILENAME,FILENAME_OUT,&fspec) == 1)
  goto out;
 if ( sds_verifiy_chunk_all(HDF_CHUNK,2,in_chunk_lengths,"dset7") == -1) 
  goto out;
 PASSED();


/*-------------------------------------------------------------------------
 * test9:  
 *-------------------------------------------------------------------------
 */

 verbose        =1;
 fspec.verbose  =1;

 TESTING("compressing SDS ALL with GZIP");
 printf("\n");
 hrepack_init (&options,verbose);
 hrepack_addcomp("*:GZIP 1",&options);
 hrepack(FILENAME,FILENAME_OUT,&options);
 hrepack_end (&options);
 if (hdiff(FILENAME,FILENAME_OUT,&fspec) == 1)
  goto out;
 if ( sds_verifiy_comp_all(COMP_CODE_DEFLATE, 1) == -1) 
  goto out;
 PASSED();
 
/*-------------------------------------------------------------------------
 * all tests PASSED
 *-------------------------------------------------------------------------
 */

 
 return 0;
out:
 H4_FAILED();
 return 1;

}
