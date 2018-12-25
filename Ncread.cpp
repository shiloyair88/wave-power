#include "stdafx.h"
#include "netcdf.h"
#include "interpolation.h"
#include <stdio.h>
#include <stdlib.h>
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}
//convert the data from netcdf file to struct point
size_t read(const char *filename,const char *varname,point **dataout)
{
	int ncid, varid, varnl = 0, varqc, longid, latid, dephid, timeid;
	if (nc_open(filename, NC_NOWRITE, &ncid))return 0;
	int nod;
	nc_inq_ndims(ncid, &nod);
	size_t *dsize = (size_t*)malloc(nod * sizeof(size_t));
	for (int index = 0; index < nod; index++)
	{
		nc_inq_dim(ncid, index, NULL, dsize + index);
	}
	if (nc_inq_varid(ncid, varname, &varid))return 0;
	//printf("varexist%d\n",varname[4]);
	while (varname[varnl])varnl++;
	//printf("varnl:%d\n", varnl);
	char *tmp = new char[varnl + 4];
	for (varnl = 0; varname[varnl]; varnl++)tmp[varnl] = varname[varnl];
	tmp[varnl] = '_';
	tmp[varnl+1] = 'Q';
	tmp[varnl+2] = 'C';
	tmp[varnl+3] = 0;
	if (nc_inq_varid(ncid, tmp, &varqc))return 0;
	if (nc_inq_varid(ncid, "TIME", &timeid))return 0;
	if (nc_inq_varid(ncid, "LONGITUDE", &longid))return 0;
	if (nc_inq_varid(ncid, "LATITUDE", &latid))return 0;
	if (nc_inq_varid(ncid, "DEPH", &dephid))return 0;
	//printf("inqend\n");
	//printf("timeid:%d\n", timeid);
	nc_type type;
	int vdid[2];
	nc_inq_var(ncid, varid, NULL, &type, NULL, vdid, NULL);
	size_t start[] = { 0,0 }, count[] = { dsize[vdid[0]],dsize[vdid[1]] }, output = 0, index = 0;
	double  *time = new double[count[0]];
	float *data = new float[count[0] * count[1]], *longitude = new float[count[0]], *latitude = new float[count[0]], *depth = new float[count[0] * count[1]];
	unsigned char *qc = new unsigned char[count[0] * count[1]];
	//printf("allocend\n");
	if (nc_get_vara(ncid, varqc, start, count, qc))return 0;
	if (nc_get_vara(ncid, varid, start, count, data))return 0;
	if (nc_get_vara(ncid, timeid, start, count, time))return 0;
	if (nc_get_vara(ncid, latid, start, count, latitude))return 0;
	if (nc_get_vara(ncid, longid, start, count, longitude))return 0;
	if (nc_get_vara(ncid, dephid, start, count, depth))return 0;
	for (size_t i = 0; i < count[0] * count[1]; i++)if (qc[i] == 1)output++;
	*dataout = new point[output];
	for (size_t i = 0; i < count[0] * count[1]; i++)if (qc[i] == 1)
	{
		point tmp;
		tmp.output = data[i];
		tmp.time = time[i / count[1]];
		tmp.x = longitude[i / count[1]];
		tmp.y = latitude[i / count[1]];
		tmp.z = depth[i];
		(*dataout)[index] = tmp;
		index++;
	}
	//printf("readend\n");
	free(dsize);
	delete[] data;
	//printf("freed\n");
	delete[] longitude;
	//printf("freelong\n");
	delete[] latitude;
	//printf("freelat\n");
	delete[] time;
	//printf("freet\n");
	delete[] depth;
	//printf("freed\n");
	delete[] qc;
	//printf("freeq\n");
	delete[] tmp;
	//printf("freetmp\n");
	//printf("free\n");
	return output;
}
//convert the data from netcdf file to struct point(for other version of netcdf structure)
size_t *read2(const char *filename, const char *varname, point **dataout)
{
	int ncid, varid, longid, latid, timeid;
	if (nc_open(filename, NC_NOWRITE, &ncid))return 0;
	int nod;
	nc_inq_ndims(ncid, &nod);
	size_t *dsize = (size_t*)malloc(nod * sizeof(size_t));
	for (int index = 0; index < nod; index++)
	{
		nc_inq_dim(ncid, index, NULL, dsize + index);
	}
	if (nc_inq_varid(ncid, varname, &varid))return 0;
	if (nc_inq_varid(ncid, "time", &timeid))return 0;
	if (nc_inq_varid(ncid, "longitude", &longid))return 0;
	if (nc_inq_varid(ncid, "latitude", &latid))return 0;
	nc_type type;
	int vdid[3], *time = new int[dsize[0]];
	nc_inq_var(ncid, varid, NULL, &type, NULL, vdid, NULL);
	size_t start[] = { 0,0,0 }, *count = new size_t[3];
	for (size_t i = 0; i < 3; i++)count[i] = dsize[vdid[i]];
	float *longitude = new float[dsize[2]], *latitude = new float[dsize[1]];
	short *value = new short[dsize[0] * dsize[1] * dsize[2]];
	if (nc_get_vara(ncid, varid, start, count, value))return 0;
	if (nc_get_vara(ncid, timeid, start, count, time))return 0;
	if (nc_get_vara(ncid, latid, start, count + 1, latitude))return 0;
	if (nc_get_vara(ncid, longid, start, count + 2, longitude))return 0;
	*dataout = new point[dsize[0] * dsize[1] * dsize[2]];
	for (size_t i = 0; i < dsize[0] * dsize[1] * dsize[2]; i++)
	{
		(*dataout)[i].output = value[i] * 0.001;
		size_t tmp = i;
		(*dataout)[i].z = 0;
		(*dataout)[i].x = longitude[tmp % count[2]];
		tmp /= count[2];
		(*dataout)[i].y = latitude[tmp%count[1]];
		tmp /= count[1];
		(*dataout)[i].time = time[tmp];
		(*dataout)[i].qc = value[i] == -32767;
	}
	free(dsize);
	delete[] time;
	delete[] latitude;
	delete[] longitude;
	delete[] value;
	return count;
}