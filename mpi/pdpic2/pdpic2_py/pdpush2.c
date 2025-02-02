/* C Library for Skeleton 2-1/2D Darwin MPI PIC Code */
/* written by Viktor K. Decyk, UCLA */

#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <math.h>
#include "pdpush2.h"
#include "pplib2.h"

/*--------------------------------------------------------------------*/
double ranorm() {
/* this program calculates a random number y from a gaussian distribution
   with zero mean and unit variance, according to the method of
   mueller and box:
      y(k) = (-2*ln(x(k)))**1/2*sin(2*pi*x(k+1))
      y(k+1) = (-2*ln(x(k)))**1/2*cos(2*pi*x(k+1)),
   where x is a random number uniformly distributed on (0,1).
   written for the ibm by viktor k. decyk, ucla
local data                                                              */
   static int r1 = 885098780, r2 = 1824280461;
   static int r4 = 1396483093, r5 = 55318673;
   static int iflg = 0;
   static double h1l = 65531.0, h1u = 32767.0, h2l = 65525.0;
   static double r0 = 0.0;
   int isc, i1;
   double ranorm, r3, asc, bsc, temp;
   if (iflg==1) {
      ranorm = r0;
      r0 = 0.0;
      iflg = 0;
      return ranorm;
   }
   isc = 65536;
   asc = (double) isc;
   bsc = asc*asc;
   i1 = r1 - (r1/isc)*isc;
   r3 = h1l*(double) r1 + asc*h1u*(double) i1;
   i1 = r3/bsc;
   r3 -= ((double) i1)*bsc;
   bsc = 0.5*bsc;
   i1 = r2/isc;
   isc = r2 - i1*isc;
   r0 = h1l*(double) r2 + asc*h1u*(double) isc;
   asc = 1.0/bsc;
   isc = r0*asc;
   r2 = r0 - ((double) isc)*bsc;
   r3 += (double) isc + 2.0*h1u*(double) i1;
   isc = r3*asc;
   r1 = r3 - ((double) isc)*bsc;
   temp = sqrt(-2.0*log((((double) r1) + ((double) r2)*asc)*asc));
   isc = 65536;
   asc = (double) isc;
   bsc = asc*asc;
   i1 = r4 - (r4/isc)*isc;
   r3 = h2l*(double) r4 + asc*h1u*(double) i1;
   i1 = r3/bsc;
   r3 -= ((double) i1)*bsc;
   bsc = 0.5*bsc;
   i1 = r5/isc;
   isc = r5 - i1*isc;
   r0 = h2l*(double) r5 + asc*h1u*(double) isc;
   asc = 1.0/bsc;
   isc = r0*asc;
   r5 = r0 - ((double) isc)*bsc;
   r3 += (double) isc + 2.0*h1u*(double) i1;
   isc = r3*asc;
   r4 = r3 - ((double) isc)*bsc;
   r0 = 6.28318530717959*((((double) r4) + ((double) r5)*asc)*asc);
   ranorm = temp*sin(r0);
   r0 = temp*cos(r0);
   iflg = 1;
   return ranorm;
}

/*--------------------------------------------------------------------*/
void cpdicomp2l(float edges[], int *nyp, int *noff, int *nypmx,
                int *nypmn, int ny, int kstrt, int nvp, int idps) {
/* this subroutine determines spatial boundaries for uniform particle
   decomposition, calculates number of grid points in each spatial
   region, and the offset of these grid points from the global address
   nvp must be < ny.  some combinations of ny and nvp result in a zero
   value of nyp.  this is not supported.
   integer boundaries are set.
   input: ny, kstrt, nvp, idps, output: edges, nyp, noff, nypmx, nypmn
   edges[0] = lower boundary of particle partition
   edges[1] = upper boundary of particle partition
   nyp = number of primary (complete) gridpoints in particle partition
   noff = lowermost global gridpoint in particle partition
   nypmx = maximum size of particle partition, including guard cells
   nypmn = minimum value of nyp
   ny = system length in y direction
   kstrt = starting data block number (processor id + 1)
   nvp = number of real or virtual processors
   idps = number of partition boundaries
local data                                                            */
   int kb, kyp;
   float at1, any;
   int mypm[2], iwork2[2];
   any = (float) ny;
/* determine decomposition */
   kb = kstrt - 1;
   kyp = (ny - 1)/nvp + 1;
   at1 = (float) kyp;
   edges[0] = at1*(float) kb;
   if (edges[0] > any)
      edges[0] = any;
   *noff = edges[0];
   edges[1] = at1*(float) (kb + 1);
   if (edges[1] > any)
      edges[1] = any;
   kb = edges[1];
   *nyp = kb - *noff;
/* find maximum/minimum partition size */
   mypm[0] = *nyp;
   mypm[1] = -(*nyp);
   cppimax(mypm,iwork2,2);
   *nypmx = mypm[0] + 1;
   *nypmn = -mypm[1];
   return;
}

/*--------------------------------------------------------------------*/
void cpdistr2h(float part[], float edges[], int *npp, int nps,
               float vtx, float vty, float vtz, float vdx, float vdy,
               float vdz, int npx, int npy, int nx, int ny, int idimp,
               int npmax, int idps, int ipbc, int *ierr) {
/* for 2-1/2d code, this subroutine calculates initial particle
   co-ordinates and velocities with uniform density and maxwellian
   velocity with drift for distributed data.
   input: all except part, ierr, output: part, npp, ierr
   part[n][0] = position x of particle n in partition
   part[n][1] = position y of particle n in partition
   part[n][2] = velocity vx of particle n in partition
   part[n][3] = velocity vy of particle n in partition
   part[n][4] = velocity vz of particle n in partition
   edges[0] = lower boundary of particle partition
   edges[1] = upper boundary of particle partition
   npp = number of particles in partition
   nps = starting address of particles in partition
   vtx/vty/vtz = thermal velocity of electrons in x/y/z direction
   vdx/vdy/vdz = drift velocity of beam electrons in x/y/z direction
   npx/npy = initial number of particles distributed in x/y direction
   nx/ny = system length in x/y direction
   idimp = size of phase space = 5
   npmax = maximum number of particles in each partition
   idps = number of partition boundaries
   ipbc = particle boundary condition = (0,1,2,3) =
   (none,2d periodic,2d reflecting,mixed reflecting/periodic)
   ierr = (0,1) = (no,yes) error condition exists
   ranorm = gaussian random number with zero mean and unit variance
   with spatial decomposition
local data                                                            */
   int j, k, npt, k1, npxyp;
   float edgelx, edgely, at1, at2, xt, yt, vxt, vyt, vzt;
   double dnpx, dnpxy, dt1;
   int ierr1[1], iwork1[1];
   double sum4[4], work4[4];
   *ierr = 0;
/* particle distribution constant */
   dnpx = (double) npx;
/* set boundary values */
   edgelx = 0.0;
   edgely = 0.0;
   at1 = (float) nx/(float) npx;
   at2 = (float) ny/(float) npy;
   if (ipbc==2) {
      edgelx = 1.0;
      edgely = 1.0;
      at1 = (float) (nx-2)/(float) npx;
      at2 = (float) (ny-2)/(float) npy;
   }
   else if (ipbc==3) {
      edgelx = 1.0;
      at1 = (float) (nx-2)/(float) npx;
   }
   npt = *npp;
/* uniform density profile */
   for (k = 0; k < npy; k++) {
      yt = edgely + at2*(((float) k) + 0.5);
      for (j = 0; j < npx; j++) {
         xt = edgelx + at1*(((float) j) + 0.5);
/* maxwellian velocity distribution */
         vxt = vtx*ranorm();
         vyt = vty*ranorm();
         vzt = vtz*ranorm();
         if ((yt >= edges[0]) && (yt < edges[1])) {
            if (npt < npmax) {
               k1 = idimp*npt;
               part[k1] = xt;
               part[1+k1] = yt;
               part[2+k1] = vxt;
               part[3+k1] = vyt;
               part[4+k1] = vzt;
               npt += 1;
            }
            else
               *ierr += 1;
         }
      }
   }
   npxyp = 0;
/* add correct drift */
   sum4[0] = 0.0;
   sum4[1] = 0.0;
   sum4[2] = 0.0;
   for (j = nps-1; j < npt; j++) {
      npxyp += 1;
      sum4[0] += part[2+idimp*j];
      sum4[1] += part[3+idimp*j];
      sum4[2] += part[4+idimp*j];
   }
   sum4[3] = npxyp;
   cppdsum(sum4,work4,4);
   dnpxy = sum4[3];
   ierr1[0] = *ierr;
   cppimax(ierr1,iwork1,1);
   *ierr = ierr1[0];
   dt1 = 1.0/dnpxy;
   sum4[0] = dt1*sum4[0] - vdx;
   sum4[1] = dt1*sum4[1] - vdy;
   sum4[2] = dt1*sum4[2] - vdz;
   for (j = nps-1; j < npt; j++) {
      part[2+idimp*j] -= sum4[0];
      part[3+idimp*j] -= sum4[1];
      part[4+idimp*j] -= sum4[2];
   }
/* process errors */
   dnpxy -= dnpx*(double) npy;
   if (dnpxy != 0.0)
      *ierr = dnpxy;
   *npp = npt;
   return;
}

/*--------------------------------------------------------------------*/
void cppgbpush23l(float part[], float fxy[], float bxy[], float edges[],
                  int npp, int noff, int ihole[], float qbm, float dt, 
                  float dtc, float *ek, int nx, int ny, int idimp,
                  int npmax, int nxv, int nypmx, int idps, int ntmax,
                  int ipbc) {
/* for 2-1/2d code, this subroutine updates particle co-ordinates and
   velocities using leap-frog scheme in time and first-order linear
   interpolation in space with magnetic field. Using the Boris Mover.
   scalar version using guard cells, for distributed data
   also determines list of particles which are leaving this processor
   117 flops/particle, 1 divide, 25 loads, 5 stores
   input: all except ihole, output: part, ihole, ek
   velocity equations used are:
   vx(t+dt/2) = rot[0]*(vx(t-dt/2) + .5*(q/m)*fx(x(t),y(t))*dt) +
      rot[1]*(vy(t-dt/2) + .5*(q/m)*fy(x(t),y(t))*dt) +
      rot[2]*(vz(t-dt/2) + .5*(q/m)*fz(x(t),y(t))*dt) +
      .5*(q/m)*fx(x(t),y(t))*dt)
   vy(t+dt/2) = rot[3]*(vx(t-dt/2) + .5*(q/m)*fx(x(t),y(t))*dt) +
      rot[4]*(vy(t-dt/2) + .5*(q/m)*fy(x(t),y(t))*dt) +
      rot[5]*(vz(t-dt/2) + .5*(q/m)*fz(x(t),y(t))*dt) +
      .5*(q/m)*fy(x(t),y(t))*dt)
   vz(t+dt/2) = rot[6]*(vx(t-dt/2) + .5*(q/m)*fx(x(t),y(t))*dt) +
      rot[7]*(vy(t-dt/2) + .5*(q/m)*fy(x(t),y(t))*dt) +
      rot(9)*(vz(t-dt/2) + .5*(q/m)*fz(x(t),y(t))*dt) +
      .5*(q/m)*fz(x(t),y(t))*dt)
   where q/m is charge/mass, and the rotation matrix is given by:
      rot[0] = (1 - (om*dt/2)**2 + 2*(omx*dt/2)**2)/(1 + (om*dt/2)**2)
      rot[1] = 2*(omz*dt/2 + (omx*dt/2)*(omy*dt/2))/(1 + (om*dt/2)**2)
      rot[2] = 2*(-omy*dt/2 + (omx*dt/2)*(omz*dt/2))/(1 + (om*dt/2)**2)
      rot[3] = 2*(-omz*dt/2 + (omx*dt/2)*(omy*dt/2))/(1 + (om*dt/2)**2)
      rot[4] = (1 - (om*dt/2)**2 + 2*(omy*dt/2)**2)/(1 + (om*dt/2)**2)
      rot[5] = 2*(omx*dt/2 + (omy*dt/2)*(omz*dt/2))/(1 + (om*dt/2)**2)
      rot[6] = 2*(omy*dt/2 + (omx*dt/2)*(omz*dt/2))/(1 + (om*dt/2)**2)
      rot[7] = 2*(-omx*dt/2 + (omy*dt/2)*(omz*dt/2))/(1 + (om*dt/2)**2)
      rot[8] = (1 - (om*dt/2)**2 + 2*(omz*dt/2)**2)/(1 + (om*dt/2)**2)
   and om**2 = omx**2 + omy**2 + omz**2
   the rotation matrix is determined by:
   omx = (q/m)*bx(x(t),y(t)), omy = (q/m)*by(x(t),y(t)), and
   omz = (q/m)*bz(x(t),y(t)).
   position equations used are:
   x(t+dt)=x(t) + vx(t+dt/2)*dt
   y(t+dt)=y(t) + vy(t+dt/2)*dt
   fx(x(t),y(t)), fy(x(t),y(t)), fz(x(t),y(t))
   bx(x(t),y(t)), by(x(t),y(t)), and bz(x(t),y(t))
   are approximated by interpolation from the nearest grid points:
   fx(x,y) = (1-dy)*((1-dx)*fx(n,m)+dx*fx(n+1,m)) + dy*((1-dx)*fx(n,m+1)
      + dx*fx(n+1,m+1))
   where n,m = leftmost grid points and dx = x-n, dy = y-m
   similarly for fy(x,y), fz(x,y), bx(x,y), by(x,y), bz(x,y)
   part[n][0] = position x of particle n in partition
   part[n][1] = position y of particle n in partition
   part[n][2] = velocity vx of particle n in partition
   part[n][3] = velocity vy of particle n in partition
   part[n][4] = velocity vz of particle n in partition
   fxy[k][j][0] = x component of force/charge at grid (j,kk)
   fxy[k][j][1] = y component of force/charge at grid (j,kk)
   fxy[k][j][2] = z component of force/charge at grid (j,kk)
   that is, convolution of electric field over particle shape
   bxy[k][j][0] = x component of magnetic field at grid (j,kk)
   bxy[k][j][1] = y component of magnetic field at grid (j,kk)
   bxy[k][j][2] = z component of magnetic field at grid (j,kk)
   that is, the convolution of magnetic field over particle shape
   where kk = k + noff
   edges[0:1] = lower:upper boundary of particle partition
   npp = number of particles in partition
   noff = lowermost global gridpoint in particle partition.
   ihole = location of hole left in particle arrays
   ihole[0] = ih, number of holes left (error, if negative)
   qbm = particle charge/mass ratio
   dt = time interval between successive calculations
   dtc = time interval between successive co-ordinate calculations
   kinetic energy/mass at time t is also calculated, using
   ek = .5*sum((vx(t-dt/2) + .5*(q/m)*fx(x(t),y(t))*dt)**2 +
        (vy(t-dt/2) + .5*(q/m)*fy(x(t),y(t))*dt)**2 + 
        .25*(vz(t+dt/2) + vz(t-dt/2))**2)
   nx/ny = system length in x/y direction
   idimp = size of phase space = 5
   npmax = maximum number of particles in each partition
   nxv = first dimension of field arrays, must be >= nx+1
   nypmx = maximum size of particle partition, including guard cells.
   idps = number of partition boundaries
   ntmax = size of hole array for particles leaving processors
   ipbc = particle boundary condition = (0,1,2,3) =
   (none,2d periodic,2d reflecting,mixed reflecting/periodic)
local data                                                            */
   int mnoff, j, nn, mm, np, mp, ih, nh, nxv3;
   float qtmh, edgelx, edgely, edgerx, edgery, dxp, dyp, amx, amy;
   float dx, dy, dz, ox, oy, oz, acx, acy, acz, omxt, omyt, omzt, omt;
   float anorm, rot1, rot2, rot3, rot4, rot5, rot6, rot7, rot8, rot9;
   double sum1;
   nxv3 = 3*nxv;
   qtmh = 0.5*qbm*dt;
   sum1 = 0.0;
/* set boundary values */
   edgelx = 0.0;
   edgely = 1.0;
   edgerx = (float) nx;
   edgery = (float) (ny-1);
   if ((ipbc==2) || (ipbc==3)) {
      edgelx = 1.0;
      edgerx = (float) (nx-1);
   }
   mnoff = noff;
   ih = 0;
   nh = 0;
   for (j = 0; j < npp; j++) {
/* find interpolation weights */
      nn = part[idimp*j];
      mm = part[1+idimp*j];
      dxp = part[idimp*j] - (float) nn;
      dyp = part[1+idimp*j] - (float) mm;
      nn = 3*nn;
      mm = nxv3*(mm - mnoff);
      amx = 1.0 - dxp;
      mp = mm + nxv3;
      amy = 1.0 - dyp;
      np = nn + 3;
/* find electric field */
      dx = dyp*(dxp*fxy[np+mp] + amx*fxy[nn+mp])
         + amy*(dxp*fxy[np+mm] + amx*fxy[nn+mm]);
      dy = dyp*(dxp*fxy[1+np+mp] + amx*fxy[1+nn+mp])
         + amy*(dxp*fxy[1+np+mm] + amx*fxy[1+nn+mm]);
      dz = dyp*(dxp*fxy[2+np+mp] + amx*fxy[2+nn+mp])
         + amy*(dxp*fxy[2+np+mm] + amx*fxy[2+nn+mm]);
/* find magnetic field */
      ox = dyp*(dxp*bxy[np+mp] + amx*bxy[nn+mp])
         + amy*(dxp*bxy[np+mm] + amx*bxy[nn+mm]);
      oy = dyp*(dxp*bxy[1+np+mp] + amx*bxy[1+nn+mp])
         + amy*(dxp*bxy[1+np+mm] + amx*bxy[1+nn+mm]);
      oz = dyp*(dxp*bxy[2+np+mp] + amx*bxy[2+nn+mp])
         + amy*(dxp*bxy[2+np+mm] + amx*bxy[2+nn+mm]);
/* calculate half impulse */
      dx *= qtmh;
      dy *= qtmh;
      dz *= qtmh;
/* half acceleration */
      acx = part[2+idimp*j] + dx;
      acy = part[3+idimp*j] + dy;
      acz = part[4+idimp*j] + dz;
/* time-centered kinetic energy */
      sum1 += (acx*acx + acy*acy + acz*acz);
/* calculate cyclotron frequency */
      omxt = qtmh*ox;
      omyt = qtmh*oy;
      omzt = qtmh*oz;
/* calculate rotation matrix */
      omt = omxt*omxt + omyt*omyt + omzt*omzt;
      anorm = 2.0/(1.0 + omt);
      omt = 0.5*(1.0 - omt);
      rot4 = omxt*omyt;
      rot7 = omxt*omzt;
      rot8 = omyt*omzt;
      rot1 = omt + omxt*omxt;
      rot5 = omt + omyt*omyt;
      rot9 = omt + omzt*omzt;
      rot2 = omzt + rot4;
      rot4 -= omzt;
      rot3 = -omyt + rot7;
      rot7 += omyt;
      rot6 = omxt + rot8;
      rot8 -= omxt;
/* new velocity */
      dx += (rot1*acx + rot2*acy + rot3*acz)*anorm;
      dy += (rot4*acx + rot5*acy + rot6*acz)*anorm;
      dz += (rot7*acx + rot8*acy + rot9*acz)*anorm;
      part[2+idimp*j] = dx;
      part[3+idimp*j] = dy;
      part[4+idimp*j] = dz;
/* new position */
      dx = part[idimp*j] + dx*dtc;
      dy = part[1+idimp*j] + dy*dtc;
/* periodic boundary conditions in x */
      if (ipbc==1) {
         if (dx < edgelx) dx += edgerx;
         if (dx >= edgerx) dx -= edgerx;
      }
/* reflecting boundary conditions */
      else if (ipbc==2) {
         if ((dx < edgelx) || (dx >= edgerx)) {
            dx = part[idimp*j];
            part[2+idimp*j] = -part[2+idimp*j];
         }
         if ((dy < edgely) || (dy >= edgery)) {
            dy = part[1+idimp*j];
            part[3+idimp*j] = -part[3+idimp*j];
         }
      }
/* mixed reflecting/periodic boundary conditions */
      else if (ipbc==3) {
         if ((dx < edgelx) || (dx >= edgerx)) {
            dx = part[idimp*j];
            part[2+idimp*j] = -part[2+idimp*j];
         }
      }
/* find particles out of bounds */
      if ((dy < edges[0]) || (dy >= edges[1])) {
         if (ih < ntmax)
            ihole[ih+1] = j + 1;
         else
            nh = 1;
         ih += 1;
      }
/* set new position */
      part[idimp*j] = dx;
      part[1+idimp*j] = dy;
   }
/* set end of file flag */
   if (nh > 0)
      ih = -ih;
   ihole[0] = ih;
/* normalize kinetic energy */
   *ek += 0.5*sum1;
   return;
}

/*--------------------------------------------------------------------*/
void cppgpost2l(float part[], float q[], int npp, int noff, float qm,
                int idimp, int npmax, int nxv, int nypmx) {
/* for 2d code, this subroutine calculates particle charge density
   using first-order linear interpolation, periodic boundaries
   scalar version using guard cells, for distributed data
   17 flops/particle, 6 loads, 4 stores
   input: all, output: q
   charge density is approximated by values at the nearest grid points
   q(n,m)=qm*(1.-dx)*(1.-dy)
   q(n+1,m)=qm*dx*(1.-dy)
   q(n,m+1)=qm*(1.-dx)*dy
   q(n+1,m+1)=qm*dx*dy
   where n,m = leftmost grid points and dx = x-n, dy = y-m
   part[n][0] = position x of particle n in partition
   part[n][1] = position y of particle n in partition
   q[k][j] = charge density at grid point (j,kk),
   where kk = k + noff
   npp = number of particles in partition
   noff = lowermost global gridpoint in particle partition.
   qm = charge on particle, in units of e
   idimp = size of phase space = 4
   npmax = maximum number of particles in each partition
   nxv = first dimension of charge array, must be >= nx+1
   nypmx = maximum size of particle partition, including guard cells.
local data                                                            */
   int  mnoff, j, nn, np, mm, mp;
   float dxp, dyp, amx, amy;
   mnoff = noff;
   for (j = 0; j < npp; j++) {
/* find interpolation weights */
      nn = part[idimp*j];
      mm = part[1+idimp*j];
      dxp = qm*(part[idimp*j] - (float) nn);
      dyp = part[1+idimp*j] - (float) mm;
      mm = nxv*(mm - mnoff);
      amx = qm - dxp;
      mp = mm + nxv;
      amy = 1.0 - dyp;
      np = nn + 1;
/* deposit charge */
      q[np+mp] += dxp*dyp;
      q[nn+mp] += amx*dyp;
      q[np+mm] += dxp*amy;
      q[nn+mm] += amx*amy;
   }
   return;
}

/*--------------------------------------------------------------------*/
void cppgjpost2l(float part[], float cu[], float edges[], int npp,
                 int noff, int ihole[], float qm, float dt, int nx,
                 int ny, int idimp, int npmax, int nxv, int nypmx,
                 int idps, int ntmax, int ipbc) {
/* for 2-1/2d code, this subroutine calculates particle current density
   using first-order linear interpolation, and distributed data.
   in addition, particle positions are advanced a half time-step
   also determines list of particles which are leaving this processor
   scalar version using guard cells, for distributed data
   35 flops/particle, 17 loads, 14 stores
   input: all except ihole, output: part, ihole, cu
   current density is approximated by values at the nearest grid points
   cu(i,n,m)=qci*(1.-dx)*(1.-dy)
   cu(i,n+1,m)=qci*dx*(1.-dy)
   cu(i,n,m+1)=qci*(1.-dx)*dy
   cu(i,n+1,m+1)=qci*dx*dy
   where n,m = leftmost grid points and dx = x-n, dy = y-m
   and qci = qm*vi, where i = x,y,z
   part[n][0] = position x of particle n in partition
   part[n][1] = position y of particle n in partition
   part[n][2] = velocity vx of particle n in partition
   part[n][3] = velocity vy of particle n in partition
   part[n][4] = velocity vz of particle n in partition
   cu[k][j][i] = ith component of current density at grid point j,kk,
   where kk = k + noff
   edges[0:1] = lower:upper boundary of particle partition
   npp = number of particles in partition
   noff = lowermost global gridpoint in particle partition.
   ihole = location of hole left in particle arrays
   ihole[0] = ih, number of holes left (error, if negative)
   qm = charge on particle, in units of e
   dt = time interval between successive calculations
   nx/ny = system length in x/y direction
   idimp = size of phase space = 5
   npmax = maximum number of particles in each partition
   nxv = first dimension of current array, must be >= nx+1
   nypmx = maximum size of particle partition, including guard cells.
   idps = number of partition boundaries
   ntmax =  size of hole array for particles leaving processors
   ipbc = particle boundary condition = (0,1,2,3) =
   (none,2d periodic,2d reflecting,mixed reflecting/periodic)
local data                                                            */
   int mnoff, j, nn, mm, np, mp, ih, nh, nxv3;
   float edgelx, edgely, edgerx, edgery, dxp, dyp, amx, amy;
   float dx, dy, vx, vy, vz;
   nxv3 = 3*nxv;
/* set boundary values */
   edgelx = 0.0;
   edgely = 1.0;
   edgerx = (float) nx;
   edgery = (float) (ny-1);
   if ((ipbc==2) || (ipbc==3)) {
      edgelx = 1.0;
      edgerx = (float) (nx-1);
   }
   mnoff = noff;
   ih = 0;
   nh = 0;
   for (j = 0; j < npp; j++) {
/* find interpolation weights */
      nn = part[idimp*j];
      mm = part[1+idimp*j];
      dxp = qm*(part[idimp*j] - (float) nn);
      dyp = part[1+idimp*j] - (float) mm;
      nn = 3*nn;
      mm = nxv3*(mm - mnoff);
      amx = qm - dxp;
      mp = mm + nxv3;
      amy = 1.0 - dyp;
      np = nn + 3;
/* deposit current */
      dx = dxp*dyp;
      dy = amx*dyp;
      vx = part[2+idimp*j];
      vy = part[3+idimp*j];
      vz = part[4+idimp*j];
      cu[np+mp] += vx*dx;
      cu[1+np+mp] += vy*dx;
      cu[2+np+mp] += vz*dx;
      dx = dxp*amy;
      cu[nn+mp] += vx*dy;
      cu[1+nn+mp] += vy*dy;
      cu[2+nn+mp] += vz*dy;
      dy = amx*amy;
      cu[np+mm] += vx*dx;
      cu[1+np+mm] += vy*dx;
      cu[2+np+mm] += vz*dx;
      cu[nn+mm] += vx*dy;
      cu[1+nn+mm] += vy*dy;
      cu[2+nn+mm] += vz*dy;
/* advance position half a time-step */
      dx = part[idimp*j] + vx*dt;
      dy = part[1+idimp*j] + vy*dt;
/* periodic boundary conditions */
      if (ipbc==1) {
         if (dx < edgelx) dx += edgerx;
         if (dx >= edgerx) dx -= edgerx;
      }
/* reflecting boundary conditions */
      else if (ipbc==2) {
         if ((dx < edgelx) || (dx >= edgerx)) {
            dx = part[idimp*j];
            part[2+idimp*j] = -part[2+idimp*j];
         }
         if ((dy < edgely) || (dy >= edgery)) {
            dy = part[1+idimp*j];
            part[3+idimp*j] = -part[3+idimp*j];
         }
      }
/* mixed reflecting/periodic boundary conditions */
      else if (ipbc==3) {
         if ((dx < edgelx) || (dx >= edgerx)) {
            dx = part[idimp*j];
            part[2+idimp*j] = -part[2+idimp*j];
         }
      }
/* find particles out of bounds */
      if ((dy < edges[0]) || (dy >= edges[1])) {
         if (ih < ntmax)
            ihole[ih+1] = j + 1;
         else
            nh = 1;
         ih += 1;
      }
/* set new position */
      part[idimp*j] = dx;
      part[1+idimp*j] = dy;
   }
/* set end of file flag */
   if (nh > 0)
      ih = -ih;
   ihole[0] = ih;
   return;
}

/*--------------------------------------------------------------------*/
void cppgmjpost2l(float part[], float amu[], int npp, int noff,
                  float qm, int idimp, int npmax, int nxv, int nypmx) {
/* for 2-1/2d code, this subroutine calculates particle momentum flux
   using first-order spline interpolation
   scalar version using guard cells, for distributed data
   51 flops/particle, 21 loads, 16 stores
   input: all, output: amu
   momentum flux is approximated by values at the nearest grid points
   amu(i,n,m)=qci*(1.-dx)*(1.-dy)
   amu(i,n+1,m)=qci*dx*(1.-dy)
   amu(i,n,m+1)=qci*(1.-dx)*dy
   amu(i,n+1,m+1)=qci*dx*dy
   where n,m = leftmost grid points and dx = x-n, dy = y-m
   and qci = qm*vj*vk, where jk = xx-yy,xy,zx,zy, for i = 0, 3
   where vj = vj(t-dt/2) and vk = vk(t-dt/2)
   part[n][0] = position x of particle n at t in partition
   part[n][1] = position y of particle n at t in partition
   part[n][2] = x velocity of particle n at t - dt/2 in partition
   part[n][3] = y velocity of particle n at t - dt/2 in partition
   part[n][4] = z velocity of particle n at t - dt/2 in partition
   amu[k][j][i] = ith component of momentum flux at grid point (j,kk),
   where kk = k + noff
   npp = number of particles in partition
   noff = lowermost global gridpoint in particle partition
   qm = charge on particle, in units of e
   idimp = size of phase space = 5
   npmax = maximum number of particles in each partition
   nxv = second dimension of flux array, must be >= nx+1
   nypmx = maximum size of particle partition, including guard cells.
local data                                                            */
   int mnoff, j, nn, mm, np, mp, nxv4;
   float dxp, dyp, amx, amy;
   float dx, dy, vx, vy, vz, v1, v2, v3, v4;
   mnoff = noff;
   nxv4 = 4*nxv;
   for (j = 0; j < npp; j++) {
/* find interpolation weights */
      nn = part[idimp*j];
      mm = part[1+idimp*j];
      dxp = qm*(part[idimp*j] - (float) nn);
      dyp = part[1+idimp*j] - (float) mm;
      nn = 4*nn;
      mm = nxv4*(mm - mnoff);
      amx = qm - dxp;
      mp = mm + nxv4;
      amy = 1.0 - dyp;
      np = nn + 4;
/* deposit momentum flux */
      dx = dxp*dyp;
      dy = amx*dyp;
      vx = part[2+idimp*j];
      vy = part[3+idimp*j];
      vz = part[4+idimp*j];
      v1 = vx*vx - vy*vy;
      v2 = vx*vy;
      v3 = vz*vx;
      v4 = vz*vy;
      amu[np+mp] += v1*dx;
      amu[1+np+mp] += v2*dx;
      amu[2+np+mp] += v3*dx;
      amu[3+np+mp] += v4*dx;
      dx = dxp*amy;
      amu[nn+mp] += v1*dy;
      amu[1+nn+mp] += v2*dy;
      amu[2+nn+mp] += v3*dy;
      amu[3+nn+mp] += v4*dy;
      dy = amx*amy;
      amu[np+mm] += v1*dx;
      amu[1+np+mm] += v2*dx;
      amu[2+np+mm] += v3*dx;
      amu[3+np+mm] += v4*dx;
      amu[nn+mm] += v1*dy;
      amu[1+nn+mm] += v2*dy;
      amu[2+nn+mm] += v3*dy;
      amu[3+nn+mm] += v4*dy;
   }
   return;
}

/*--------------------------------------------------------------------*/
void cppgdjpost2l(float part[], float fxy[], float bxy[], int npp,
                  int noff, float dcu[], float amu[], float qm,
                  float qbm, float dt, int idimp, int npmax, int nxv,
                  int nypmx) {
/* for 2-1/2d code, this subroutine calculates particle momentum flux
   and acceleration density using first-order spline interpolation.
   scalar version using guard cells, for distributed data
   194 flops/particle, 1 divide, 57 loads, 28 stores
   input: all, output: dcu, amu
   acceleration density is approximated by values at the nearest grid
   points
   dcu(i,n,m)=qci*(1.-dx)*(1.-dy)
   dcu(i,n+1,m)=qci*dx*(1.-dy)
   dcu(i,n,m+1)=qci*(1.-dx)*dy
   dcu(i,n+1,m+1)=qci*dx*dy
   and qci = qm*dvj/dt, where j = x,y,z, for i = 1, 3
   where dvj = (vj(t+dt/2)-vj(t-dt/2))/dt
   momentum flux is approximated by values at the nearest grid points
   amu(i,n,m)=qci*(1.-dx)*(1.-dy)
   amu(i,n+1,m)=qci*dx*(1.-dy)
   amu(i,n,m+1)=qci*(1.-dx)*dy
   amu(i,n+1,m+1)=qci*dx*dy
   and qci = qm*vj*vk, where jk = xx-yy,xy,zx,zy, for i = 1, 4
   where vj = 0.5*(vj(t+dt/2)+vj(t-dt/2),
   and vk = 0.5*(vk(t+dt/2)+vk(t-dt/2))
   where n,m = nearest grid points and dx = x-n, dy = y-m
   velocity equations at t=t+dt/2 are calculated from:
   vx(t+dt/2) = rot(1)*(vx(t-dt/2) + .5*(q/m)*fx(x(t),y(t))*dt) +
      rot(2)*(vy(t-dt/2) + .5*(q/m)*fy(x(t),y(t))*dt) +
      rot(3)*(vz(t-dt/2) + .5*(q/m)*fz(x(t),y(t))*dt) +
      .5*(q/m)*fx(x(t),y(t))*dt)
   vy(t+dt/2) = rot(4)*(vx(t-dt/2) + .5*(q/m)*fx(x(t),y(t))*dt) +
      rot(5)*(vy(t-dt/2) + .5*(q/m)*fy(x(t),y(t))*dt) +
      rot(6)*(vz(t-dt/2) + .5*(q/m)*fz(x(t),y(t))*dt) +
      .5*(q/m)*fy(x(t),y(t))*dt)
   vz(t+dt/2) = rot(7)*(vx(t-dt/2) + .5*(q/m)*fx(x(t),y(t))*dt) +
      rot(8)*(vy(t-dt/2) + .5*(q/m)*fy(x(t),y(t))*dt) +
      rot(9)*(vz(t-dt/2) + .5*(q/m)*fz(x(t),y(t))*dt) +
      .5*(q/m)*fz(x(t),y(t))*dt)
   where q/m is charge/mass, and the rotation matrix is given by:
      rot(1) = (1 - (om*dt/2)**2 + 2*(omx*dt/2)**2)/(1 + (om*dt/2)**2)
      rot(2) = 2*(omz*dt/2 + (omx*dt/2)*(omy*dt/2))/(1 + (om*dt/2)**2)
      rot(3) = 2*(-omy*dt/2 + (omx*dt/2)*(omz*dt/2))/(1 + (om*dt/2)**2)
      rot(4) = 2*(-omz*dt/2 + (omx*dt/2)*(omy*dt/2))/(1 + (om*dt/2)**2)
      rot(5) = (1 - (om*dt/2)**2 + 2*(omy*dt/2)**2)/(1 + (om*dt/2)**2)
      rot(6) = 2*(omx*dt/2 + (omy*dt/2)*(omz*dt/2))/(1 + (om*dt/2)**2)
      rot(7) = 2*(omy*dt/2 + (omx*dt/2)*(omz*dt/2))/(1 + (om*dt/2)**2)
      rot(8) = 2*(-omx*dt/2 + (omy*dt/2)*(omz*dt/2))/(1 + (om*dt/2)**2)
      rot(9) = (1 - (om*dt/2)**2 + 2*(omz*dt/2)**2)/(1 + (om*dt/2)**2)
   and om**2 = omx**2 + omy**2 + omz**2
   the rotation matrix is determined by:
   omx = (q/m)*bx(x(t),y(t)), omy = (q/m)*by(x(t),y(t)), and
   omz = (q/m)*bz(x(t),y(t)).
   fx(x(t),y(t)), fy(x(t),y(t)), and fz(x(t),y(t))
   bx(x(t),y(t)), by(x(t),y(t)), and bz(x(t),y(t))
   are approximated by interpolation from the nearest grid points:
   fx(x,y) = (1-dy)*((1-dx)*fx(n,m)+dx*fx(n+1,m)) + dy*((1-dx)*fx(n,m+1)
      + dx*fx(n+1,m+1))
   where n,m = leftmost grid points and dx = x-n, dy = y-m
   similarly for fy(x,y), fz(x,y), bx(x,y), by(x,y), bz(x,y)
   part[n][0] = position x of particle n at t in partition
   part[n][1] = position y of particle n at t in partition
   part[n][2] = velocity vx of particle n at t - dt/2 in partition
   part[n][3] = velocity vy of particle n at t - dt/2 in partition
   part[n][4] = velocity vz of particle n at t - dt/2 in partition
   fxy[k][j][0] = x component of force/charge at grid (j,kk)
   fxy[k][j][1] = y component of force/charge at grid (j,kk)
   fxy[k][j][2] = z component of force/charge at grid (j,kk)
   that is, convolution of electric field over particle shape
   where kk = k + noff - 1
   that is, convolution of electric field over particle shape
   bxy[k][j][0] = x component of magnetic field at grid (j,kk)
   bxy[k][j][1] = y component of magnetic field at grid (j,kk)
   bxy[k][j][2] = z component of magnetic field at grid (j,kk)
   that is, the convolution of magnetic field over particle shape
   where kk = k + noff
   npp = number of particles in partition
   noff = lowermost global gridpoint in particle partition
   dcu[k][j][i] = ith component of acceleration density
   at grid point j,kk for i = 0, 2
   amu[k][j][i] = ith component of momentum flux
   at grid point j,kk for i = 0, 3
   qm = charge on particle, in units of e
   qbm = particle charge/mass ratio
   dt = time interval between successive calculations
   idimp = size of phase space = 5
   npmax = maximum number of particles in each partition
   nxv = second dimension of field arrays, must be >= nx+1
   nypmx = maximum size of particle partition, including guard cells.
local data                                                            */
   int mnoff, j, nn, mm, np, mp, nnn, mmm, nnp, mmp, nxv3, nxv4;
   float qtmh, dti, dxp, dyp, amx, amy, dx, dy, dz, ox, oy, oz;
   float acx, acy, acz, omxt, omyt, omzt, omt, anorm;
   float rot1, rot2, rot3, rot4, rot5, rot6, rot7, rot8, rot9;
   float vx, vy, vz, v1, v2, v3, v4;
   nxv3 = 3*nxv;
   nxv4 = 4*nxv;
   qtmh = 0.5*qbm*dt;
   dti = 1.0/dt;
   mnoff = noff;
   for (j = 0; j < npp; j++) {
/* find interpolation weights */
      nnn = part[idimp*j];
      mmm = part[1+idimp*j];
      dxp = part[idimp*j] - (float) nnn;
      dyp = part[1+idimp*j] - (float) mmm;
      nn = 3*nnn;
      mm = nxv3*(mmm - mnoff);
      amx = 1.0 - dxp;
      mp = mm + nxv3;
      amy = 1.0 - dyp;
      np = nn + 3;
      nnn = 4*nnn;
      mmm = nxv4*(mmm - mnoff);
      mmp = mmm + nxv4;
      nnp = nnn + 4;
/* find electric field */
      dx = dyp*(dxp*fxy[np+mp] + amx*fxy[nn+mp])
         + amy*(dxp*fxy[np+mm] + amx*fxy[nn+mm]);
      dy = dyp*(dxp*fxy[1+np+mp] + amx*fxy[1+nn+mp])
         + amy*(dxp*fxy[1+np+mm] + amx*fxy[1+nn+mm]);
      dz = dyp*(dxp*fxy[2+np+mp] + amx*fxy[2+nn+mp])
         + amy*(dxp*fxy[2+np+mm] + amx*fxy[2+nn+mm]);
/* find magnetic field */
      ox = dyp*(dxp*bxy[np+mp] + amx*bxy[nn+mp])
         + amy*(dxp*bxy[np+mm] + amx*bxy[nn+mm]);
      oy = dyp*(dxp*bxy[1+np+mp] + amx*bxy[1+nn+mp])
         + amy*(dxp*bxy[1+np+mm] + amx*bxy[1+nn+mm]);
      oz = dyp*(dxp*bxy[2+np+mp] + amx*bxy[2+nn+mp])
         + amy*(dxp*bxy[2+np+mm] + amx*bxy[2+nn+mm]);
/* calculate half impulse */
      dx *= qtmh;
      dy *= qtmh;
      dz *= qtmh;
/* half acceleration */
      vx = part[2+idimp*j];
      vy = part[3+idimp*j];
      vz = part[4+idimp*j];
      acx = vx + dx;
      acy = vy + dy;
      acz = vz + dz;
/* calculate cyclotron frequency */
      omxt = qtmh*ox;
      omyt = qtmh*oy;
      omzt = qtmh*oz;
/* calculate rotation matrix */
      omt = omxt*omxt + omyt*omyt + omzt*omzt;
      anorm = 2.0/(1.0 + omt);
      omt = 0.5*(1.0 - omt);
      rot4 = omxt*omyt;
      rot7 = omxt*omzt;
      rot8 = omyt*omzt;
      rot1 = omt + omxt*omxt;
      rot5 = omt + omyt*omyt;
      rot9 = omt + omzt*omzt;
      rot2 = omzt + rot4;
      rot4 -= omzt;
      rot3 = -omyt + rot7;
      rot7 += omyt;
      rot6 = omxt + rot8;
      rot8 -= omxt;
/* new velocity */
      dx += (rot1*acx + rot2*acy + rot3*acz)*anorm;
      dy += (rot4*acx + rot5*acy + rot6*acz)*anorm;
      dz += (rot7*acx + rot8*acy + rot9*acz)*anorm;
/* deposit momentum flux and acceleration density */
      amx = qm*amx;
      dxp = qm*dxp;
      ox = 0.5*(dx + vx);
      oy = 0.5*(dy + vy);
      oz = 0.5*(dz + vz);
      vx = dti*(dx - vx);
      vy = dti*(dy - vy);
      vz = dti*(dz - vz);
      dx = dxp*dyp;
      dy = amx*dyp;
      v1 = ox*ox - oy*oy;
      v2 = ox*oy;
      v3 = oz*ox;
      v4 = oz*oy;
      amu[nnp+mmp] += v1*dx;
      amu[1+nnp+mmp] += v2*dx;
      amu[2+nnp+mmp] += v3*dx;
      amu[3+nnp+mmp] += v4*dx;
      dcu[np+mp] += vx*dx;
      dcu[1+np+mp] += vy*dx;
      dcu[2+np+mp] += vz*dx;
      dx = dxp*amy;
      amu[nnn+mmp] += v1*dy;
      amu[1+nnn+mmp] += v2*dy;
      amu[2+nnn+mmp] += v3*dy;
      amu[3+nnn+mmp] += v4*dy;
      dcu[nn+mp] += vx*dy;
      dcu[1+nn+mp] += vy*dy;
      dcu[2+nn+mp] += vz*dy;
      dy = amx*amy;
      amu[nnp+mmm] += v1*dx;
      amu[1+nnp+mmm] += v2*dx;
      amu[2+nnp+mmm] += v3*dx;
      amu[3+nnp+mmm] += v4*dx;
      dcu[np+mm] += vx*dx;
      dcu[1+np+mm] += vy*dx;
      dcu[2+np+mm] += vz*dx;
      amu[nnn+mmm] += v1*dy;
      amu[1+nnn+mmm] += v2*dy;
      amu[2+nnn+mmm] += v3*dy;
      amu[3+nnn+mmm] += v4*dy;
      dcu[nn+mm] += vx*dy;
      dcu[1+nn+mm] += vy*dy;
      dcu[2+nn+mm] += vz*dy;
   }
   return;
}

/*--------------------------------------------------------------------*/
void cppgdcjpost2l(float part[], float fxy[], float bxy[], int npp,
                   int noff, float cu[], float dcu[], float amu[],
                   float qm, float qbm, float dt, int idimp, int npmax,
                   int nxv, int nypmx) {
/*  for 2-1/2d code, this subroutine calculates particle momentum flux,
   acceleration density and current density using first-order spline
   interpolation.
   scalar version using guard cells, for distributed data
   218 flops/particle, 1 divide, 69 loads, 40 stores
   input: all, output: cu, dcu, amu
   current density is approximated by values at the nearest grid points
   cu(i,n,m)=qci*(1.-dx)*(1.-dy)
   cu(i,n+1,m)=qci*dx*(1.-dy)
   cu(i,n,m+1)=qci*(1.-dx)*dy
   cu(i,n+1,m+1)=qci*dx*dy
   and qci = qm*vj, where j = x,y,z, for i = 1, 3
   where vj = .5*(vj(t+dt/2)+vj(t-dt/2))
   acceleration density is approximated by values at the nearest grid
   points
   dcu(i,n,m)=qci*(1.-dx)*(1.-dy)
   dcu(i,n+1,m)=qci*dx*(1.-dy)
   dcu(i,n,m+1)=qci*(1.-dx)*dy
   dcu(i,n+1,m+1)=qci*dx*dy
   and qci = qm*dvj/dt, where j = x,y,z, for i = 1, 3
   where dvj = (vj(t+dt/2)-vj(t-dt/2))/dt
   momentum flux is approximated by values at the nearest grid points
   amu(i,n,m)=qci*(1.-dx)*(1.-dy)
   amu(i,n+1,m)=qci*dx*(1.-dy)
   amu(i,n,m+1)=qci*(1.-dx)*dy
   amu(i,n+1,m+1)=qci*dx*dy
   and qci = qm*vj*vk, where jk = xx-yy,xy,zx,zy, for i = 1, 4
   where vj = 0.5*(vj(t+dt/2)+vj(t-dt/2),
   and vk = 0.5*(vk(t+dt/2)+vk(t-dt/2))
   where n,m = nearest grid points and dx = x-n, dy = y-m
   velocity equations at t=t+dt/2 are calculated from:
   vx(t+dt/2) = rot(1)*(vx(t-dt/2) + .5*(q/m)*fx(x(t),y(t))*dt) +
      rot(2)*(vy(t-dt/2) + .5*(q/m)*fy(x(t),y(t))*dt) +
      rot(3)*(vz(t-dt/2) + .5*(q/m)*fz(x(t),y(t))*dt) +
      .5*(q/m)*fx(x(t),y(t))*dt)
   vy(t+dt/2) = rot(4)*(vx(t-dt/2) + .5*(q/m)*fx(x(t),y(t))*dt) +
      rot(5)*(vy(t-dt/2) + .5*(q/m)*fy(x(t),y(t))*dt) +
      rot(6)*(vz(t-dt/2) + .5*(q/m)*fz(x(t),y(t))*dt) +
      .5*(q/m)*fy(x(t),y(t))*dt)
   vz(t+dt/2) = rot(7)*(vx(t-dt/2) + .5*(q/m)*fx(x(t),y(t))*dt) +
      rot(8)*(vy(t-dt/2) + .5*(q/m)*fy(x(t),y(t))*dt) +
      rot(9)*(vz(t-dt/2) + .5*(q/m)*fz(x(t),y(t))*dt) +
      .5*(q/m)*fz(x(t),y(t))*dt)
   where q/m is charge/mass, and the rotation matrix is given by:
      rot(1) = (1 - (om*dt/2)**2 + 2*(omx*dt/2)**2)/(1 + (om*dt/2)**2)
      rot(2) = 2*(omz*dt/2 + (omx*dt/2)*(omy*dt/2))/(1 + (om*dt/2)**2)
      rot(3) = 2*(-omy*dt/2 + (omx*dt/2)*(omz*dt/2))/(1 + (om*dt/2)**2)
      rot(4) = 2*(-omz*dt/2 + (omx*dt/2)*(omy*dt/2))/(1 + (om*dt/2)**2)
      rot(5) = (1 - (om*dt/2)**2 + 2*(omy*dt/2)**2)/(1 + (om*dt/2)**2)
      rot(6) = 2*(omx*dt/2 + (omy*dt/2)*(omz*dt/2))/(1 + (om*dt/2)**2)
      rot(7) = 2*(omy*dt/2 + (omx*dt/2)*(omz*dt/2))/(1 + (om*dt/2)**2)
      rot(8) = 2*(-omx*dt/2 + (omy*dt/2)*(omz*dt/2))/(1 + (om*dt/2)**2)
      rot(9) = (1 - (om*dt/2)**2 + 2*(omz*dt/2)**2)/(1 + (om*dt/2)**2)
   and om**2 = omx**2 + omy**2 + omz**2
   the rotation matrix is determined by:
   omx = (q/m)*bx(x(t),y(t)), omy = (q/m)*by(x(t),y(t)), and
   omz = (q/m)*bz(x(t),y(t)).
   fx(x(t),y(t)), fy(x(t),y(t)), and fz(x(t),y(t))
   bx(x(t),y(t)), by(x(t),y(t)), and bz(x(t),y(t))
   are approximated by interpolation from the nearest grid points:
   fx(x,y) = (1-dy)*((1-dx)*fx(n,m)+dx*fx(n+1,m)) + dy*((1-dx)*fx(n,m+1)
      + dx*fx(n+1,m+1))
   where n,m = leftmost grid points and dx = x-n, dy = y-m
   similarly for fy(x,y), fz(x,y), bx(x,y), by(x,y), bz(x,y)
   part[n][0] = position x of particle n at t in partition
   part[n][1] = position y of particle n at t in partition
   part[n][2] = velocity vx of particle n at t - dt/2 in partition
   part[n][3] = velocity vy of particle n at t - dt/2 in partition
   part[n][4] = velocity vz of particle n at t - dt/2 in partition
   fxy[k][j][0] = x component of force/charge at grid (j,kk)
   fxy[k][j][1] = y component of force/charge at grid (j,kk)
   fxy[k][j][2] = z component of force/charge at grid (j,kk)
   that is, convolution of electric field over particle shape
   where kk = k + noff - 1
   that is, convolution of electric field over particle shape
   bxy[k][j][0] = x component of magnetic field at grid (j,kk)
   bxy[k][j][1] = y component of magnetic field at grid (j,kk)
   bxy[k][j][2] = z component of magnetic field at grid (j,kk)
   that is, the convolution of magnetic field over particle shape
   where kk = k + noff
   npp = number of particles in partition
   noff = lowermost global gridpoint in particle partition
   cu[k][j][i] = ith component of current density
   at grid point j,kk for i = 0, 2
   dcu[k][j][i] = ith component of acceleration density
   at grid point j,kk for i = 0, 2
   amu[k][j][i] = ith component of momentum flux
   at grid point j,kk for i = 0, 3
   qm = charge on particle, in units of e
   qbm = particle charge/mass ratio
   dt = time interval between successive calculations
   idimp = size of phase space = 5
   npmax = maximum number of particles in each partition
   nxv = second dimension of field arrays, must be >= nx+1
   nypmx = maximum size of particle partition, including guard cells.
local data                                                            */
   int mnoff, j, nn, mm, np, mp, nnn, mmm, nnp, mmp, nxv3, nxv4;
   float qtmh, dti, dxp, dyp, amx, amy, dx, dy, dz, ox, oy, oz;
   float acx, acy, acz, omxt, omyt, omzt, omt, anorm;
   float rot1, rot2, rot3, rot4, rot5, rot6, rot7, rot8, rot9;
   float vx, vy, vz, v1, v2, v3, v4;
   nxv3 = 3*nxv;
   nxv4 = 4*nxv;
   qtmh = 0.5*qbm*dt;
   dti = 1.0/dt;
   mnoff = noff;
   for (j = 0; j < npp; j++) {
/* find interpolation weights */
      nnn = part[idimp*j];
      mmm = part[1+idimp*j];
      dxp = part[idimp*j] - (float) nnn;
      dyp = part[1+idimp*j] - (float) mmm;
      nn = 3*nnn;
      mm = nxv3*(mmm - mnoff);
      amx = 1.0 - dxp;
      mp = mm + nxv3;
      amy = 1.0 - dyp;
      np = nn + 3;
      nnn = 4*nnn;
      mmm = nxv4*(mmm - mnoff);
      mmp = mmm + nxv4;
      nnp = nnn + 4;
/* find electric field */
      dx = dyp*(dxp*fxy[np+mp] + amx*fxy[nn+mp])
         + amy*(dxp*fxy[np+mm] + amx*fxy[nn+mm]);
      dy = dyp*(dxp*fxy[1+np+mp] + amx*fxy[1+nn+mp])
         + amy*(dxp*fxy[1+np+mm] + amx*fxy[1+nn+mm]);
      dz = dyp*(dxp*fxy[2+np+mp] + amx*fxy[2+nn+mp])
         + amy*(dxp*fxy[2+np+mm] + amx*fxy[2+nn+mm]);
/* find magnetic field */
      ox = dyp*(dxp*bxy[np+mp] + amx*bxy[nn+mp])
         + amy*(dxp*bxy[np+mm] + amx*bxy[nn+mm]);
      oy = dyp*(dxp*bxy[1+np+mp] + amx*bxy[1+nn+mp])
         + amy*(dxp*bxy[1+np+mm] + amx*bxy[1+nn+mm]);
      oz = dyp*(dxp*bxy[2+np+mp] + amx*bxy[2+nn+mp])
         + amy*(dxp*bxy[2+np+mm] + amx*bxy[2+nn+mm]);
/* calculate half impulse */
      dx *= qtmh;
      dy *= qtmh;
      dz *= qtmh;
/* half acceleration */
      vx = part[2+idimp*j];
      vy = part[3+idimp*j];
      vz = part[4+idimp*j];
      acx = vx + dx;
      acy = vy + dy;
      acz = vz + dz;
/* calculate cyclotron frequency */
      omxt = qtmh*ox;
      omyt = qtmh*oy;
      omzt = qtmh*oz;
/* calculate rotation matrix */
      omt = omxt*omxt + omyt*omyt + omzt*omzt;
      anorm = 2.0/(1.0 + omt);
      omt = 0.5*(1.0 - omt);
      rot4 = omxt*omyt;
      rot7 = omxt*omzt;
      rot8 = omyt*omzt;
      rot1 = omt + omxt*omxt;
      rot5 = omt + omyt*omyt;
      rot9 = omt + omzt*omzt;
      rot2 = omzt + rot4;
      rot4 -= omzt;
      rot3 = -omyt + rot7;
      rot7 += omyt;
      rot6 = omxt + rot8;
      rot8 -= omxt;
/* new velocity */
      dx += (rot1*acx + rot2*acy + rot3*acz)*anorm;
      dy += (rot4*acx + rot5*acy + rot6*acz)*anorm;
      dz += (rot7*acx + rot8*acy + rot9*acz)*anorm;
/* deposit momentum flux, acceleration density, and current density */
      amx = qm*amx;
      dxp = qm*dxp;
      ox = 0.5*(dx + vx);
      oy = 0.5*(dy + vy);
      oz = 0.5*(dz + vz);
      vx = dti*(dx - vx);
      vy = dti*(dy - vy);
      vz = dti*(dz - vz);
      dx = dxp*dyp;
      dy = amx*dyp;
      v1 = ox*ox - oy*oy;
      v2 = ox*oy;
      v3 = oz*ox;
      v4 = oz*oy;
      amu[nnp+mmp] += v1*dx;
      amu[1+nnp+mmp] += v2*dx;
      amu[2+nnp+mmp] += v3*dx;
      amu[3+nnp+mmp] += v4*dx;
      dcu[np+mp] += vx*dx;
      dcu[1+np+mp] += vy*dx;
      dcu[2+np+mp] += vz*dx;
      cu[np+mp] += ox*dx;
      cu[1+np+mp] += oy*dx;
      cu[2+np+mp] += oz*dx;
      dx = dxp*amy;
      amu[nnn+mmp] += v1*dy;
      amu[1+nnn+mmp] += v2*dy;
      amu[2+nnn+mmp] += v3*dy;
      amu[3+nnn+mmp] += v4*dy;
      dcu[nn+mp] += vx*dy;
      dcu[1+nn+mp] += vy*dy;
      dcu[2+nn+mp] += vz*dy;
      cu[nn+mp] += ox*dy;
      cu[1+nn+mp] += oy*dy;
      cu[2+nn+mp] += oz*dy;
      dy = amx*amy;
      amu[nnp+mmm] += v1*dx;
      amu[1+nnp+mmm] += v2*dx;
      amu[2+nnp+mmm] += v3*dx;
      amu[3+nnp+mmm] += v4*dx;
      dcu[np+mm] += vx*dx;
      dcu[1+np+mm] += vy*dx;
      dcu[2+np+mm] += vz*dx;
      cu[np+mm] += ox*dx;
      cu[1+np+mm] += oy*dx;
      cu[2+np+mm] += oz*dx;
      amu[nnn+mmm] += v1*dy;
      amu[1+nnn+mmm] += v2*dy;
      amu[2+nnn+mmm] += v3*dy;
      amu[3+nnn+mmm] += v4*dy;
      dcu[nn+mm] += vx*dy;
      dcu[1+nn+mm] += vy*dy;
      dcu[2+nn+mm] += vz*dy;
      cu[nn+mm] += ox*dy;
      cu[1+nn+mm] += oy*dy;
      cu[2+nn+mm] += oz*dy;
   }
   return;
}

/*--------------------------------------------------------------------*/
void cppdsortp2yl(float parta[], float partb[], int npic[], int npp,
                  int noff, int nyp, int idimp, int npmax, int nypm1) {
/* this subroutine sorts particles by y grid
   linear interpolation, spatial decomposition in y direction
   parta/partb = input/output particle array
   part[n][1] = position y of particle n in partition
   npic = address offset for reordering particles
   npp = number of particles in partition
   noff = backmost global gridpoint in particle partition
   nyp = number of primary gridpoints in particle partition
   idimp = size of phase space
   npmax = maximum number of particles in each partition
   nypm1 = maximum size of particle partition plus one
local data                                                            */
   int i, j, k, m, mnoff, nyp1, isum, ist, ip;
   mnoff = noff;
   nyp1 = nyp + 1;
/* clear counter array */
   for (k = 0; k < nyp1; k++) {
      npic[k] = 0;
   }
/* find how many particles in each grid */
   for (j = 0; j < npp; j++) {
      m = parta[1+idimp*j];
      m -= mnoff;
      npic[m] += 1;
   }
/* find address offset */
   isum = 0;
   for (k = 0; k < nyp1; k++) {
      ist = npic[k];
      npic[k] = isum;
      isum += ist;
   }
/* find addresses of particles at each grid and reorder particles */
   for (j = 0; j < npp; j++) {
      m = parta[1+idimp*j];
      m -= mnoff;
      ip = npic[m];
      for (i = 0; i < idimp; i++) {
         partb[i+idimp*ip] = parta[i+idimp*j];
      }
      npic[m] = ip + 1;
   }
   return;
}

/*--------------------------------------------------------------------*/
void cppcguard2xl(float fxy[], int myp, int nx, int ndim, int nxe,
                  int nypmx) {
/* replicate extended periodic vector field in x direction
   linear interpolation, for distributed data
   myp = number of full or partial grids in particle partition
   nx = system length in x direction
   ndim = leading dimension of array fxy
   nxe = first dimension of field arrays, must be >= nx+1
   nypmx = maximum size of particle partition, including guard cells
local data                                                 */
   int i, k, kk, myp1;
/* replicate edges of extended field */
   myp1 = myp + 1;
   for (k = 0; k < myp1; k++) {
      kk = ndim*nxe*k;
      for (i = 0; i < ndim; i++) {
         fxy[i+ndim*nx+kk] = fxy[i+kk];
      }
   }
   return;
}

/*--------------------------------------------------------------------*/
void cppaguard2xl(float q[], int myp, int nx, int nxe, int nypmx) {
/* accumulate extended periodic scalar field in x direction
   linear interpolation, for distributed data
   myp = number of full or partial grids in particle partition
   nx = system length in x direction
   nxe = first dimension of field arrays, must be >= nx+1
   nypmx = maximum size of particle partition, including guard cells
local data                                                 */
   int k, myp1;
/* accumulate edges of extended field */
   myp1 = myp + 1;
   for (k = 0; k < myp1; k++) {
      q[nxe*k] += q[nx+nxe*k];
      q[nx+nxe*k] = 0.0;
   }
   return;
}

/*--------------------------------------------------------------------*/
void cppacguard2xl(float cu[], int myp, int nx, int ndim, int nxe,
                   int nypmx) {
/* accumulate extended periodic vector field in x direction
   linear interpolation, for distributed data
   myp = number of full or partial grids in particle partition
   nx = system length in x direction
   ndim = leading dimension of array fxy
   nxe = first dimension of field arrays, must be >= nx+1
   nypmx = maximum size of particle partition, including guard cells
      implicit none
      real cu
      integer myp, nx, ndim, nxe, nypmx
      dimension cu(ndim,nxe,nypmx)
local data                                                 */
   int i, k, kk, myp1;
/* accumulate edges of extended field */
   myp1 = myp + 1;
   for (k = 0; k < myp1; k++) {
      kk = ndim*nxe*k;
      for (i = 0; i < ndim; i++) {
         cu[i+kk] += cu[i+ndim*nx+kk];
         cu[i+ndim*nx+kk] = 0.0;
      }
   }
   return;
}

/*--------------------------------------------------------------------*/
void cppascfguard2l(float dcu[], float cus[], int nyp, float q2m0,
                    int nx, int nxe, int nypmx) {
/* add scaled field to extended periodic field
   linear interpolation, for distributed data
   nyp = number of primary (complete) gridpoints in particle partition
   q2m0 = wp0/affp, where
   wp0 = normalized total plasma frequency squared
   affp = normalization constant = nx*ny/np, where np=number of particles
   nx = system length in x direction
   nxe = first dimension of field arrays, must be >= nx+1
   nypmx = maximum size of particle partition, including guard cells
local data                                                 */
   int i, j, k;
   for (k = 0; k < nyp; k++) {
      for (j = 0; j < nx; j++) {
         for (i = 0; i < 3; i++) {
            dcu[i+3*j+3*nxe*k] -= q2m0*cus[i+3*j+3*nxe*k];
         }
      }
   }
   return;
}

/*--------------------------------------------------------------------*/
void cppfwpminmx2(float qe[], int nyp, float qbme, float *wpmax,
                  float *wpmin, int nx, int nxe, int nypmx) {
/* calculates maximum and minimum plasma frequency.  assumes guard cells
   have already been added
   qe = charge density for electrons
   nyp = number of primary gridpoints in particle partition
   qbme = charge/mass ratio for electrons
   wpmax/wpmin = maximum/minimum plasma frequency
   nx = system length in x direction
   nxe = first dimension of field array, must be >= nx
   nypmx = maximum size of particle partition, including guard cells.
local data                                                 */
   int j, k;
   float tpmax, tpmin, at1;
   tpmax = qbme*qe[0];
   tpmin = tpmax;
   for (k = 0; k < nyp; k++) {
      for (j = 0; j < nx; j++) {
         at1 = qbme*qe[j+nxe*k];
         tpmax = at1 > tpmax ? at1 : tpmax;
         tpmin = at1 < tpmin ? at1 : tpmin;
      }
   }
   *wpmax = tpmax;
   *wpmin = tpmin;
   return;
}

/*--------------------------------------------------------------------*/
void cppois23(float complex q[], float complex fxy[], int isign,
              float complex ffc[], float ax, float ay, float affp,
              float *we, int nx, int ny, int kstrt, int nyv, int kxp,
              int nyhd) {
/* this subroutine solves 2d poisson's equation in fourier space for
   force/charge (or convolution of electric field over particle shape)
   with periodic boundary conditions.  Zeros out z component.
   for distributed data.
   for isign = 0, input: isign,ax,ay,affp,nx,ny,kstrt,nyv,kxp,nyhd,
   output: ffc
   for isign /= 0, input: q,ffc,isign,nx,ny,kstrt,nyv,kxp,nyhd,
   output: fxy,we
   approximate flop count is: 33*nxc*nyc + 15*(nxc + nyc)
   where nxc = (nx/2-1)/nvp, nyc = ny/2 - 1, and nvp = number of procs
   the equation used is:
   fx[ky][kx] = -sqrt(-1)*kx*g(kx,ky)*s(kx,ky)*q(kx,ky),
   fy[ky][kx] = -sqrt(-1)*ky*g(kx,ky)*s(kx,ky)*q(kx,ky),
   fz[ky][kx] = zero,
   where kx = 2pi*j/nx, ky = 2pi*k/ny, and j,k = fourier mode numbers,
   g[ky][kx] = (affp/(kx**2+ky**2))*s(kx,ky),
   s[ky][kx] = exp(-((kx*ax)**2+(ky*ay)**2)/2), except for
   fx(kx=pi) = fy(kx=pi) = fx(ky=pi) = fy(ky=pi) = 0, and
   fx(kx=0,ky=0) = fy(kx=0,ky=0) = 0.
   q[k][j] = complex charge density for fourier mode (jj,k)
   fxy[k][j][0] = x component of complex force/charge,
   fxy[k][j][1] = y component of complex force/charge,
   fxy[k][j][2] = zero,
   for fourier mode (jj,k), where jj = j + kxp*(kstrt - 1)
   kxp = number of data values per block
   kstrt = starting data block number
   if isign = 0, form factor array is prepared
   if isign is not equal to 0, force/charge is calculated.
   aimag(ffc[k][j]) = finite-size particle shape factor s
   real(ffc[k][j])) = potential green's function g
   for fourier mode (jj,k), where jj = j + kxp*(kstrt - 1)
   ax/ay = half-width of particle in x/y direction
   affp = normalization constant = nx*ny/np, where np=number of particles
   electric field energy is also calculated, using
   we = nx*ny*sum((affp/(kx**2+ky**2))*|q(kx,ky)*s(kx,ky)|**2)
   nx/ny = system length in x/y direction
   nyv = first dimension of field arrays, must be >= ny
   nyhd = first dimension of form factor array, must be >= nyh
local data                                                 */
   int nxh, nyh, ks, joff, kxps, j, jj, jk, k, k1;
   float dnx, dny, dkx, dky, at1, at2, at3, at4;
   float complex zero, zt1, zt2;
   double wp;
   nxh = nx/2;
   nyh = 1 > ny/2 ? 1 : ny/2;
   ks = kstrt - 1;
   joff = kxp*ks;
   kxps = nxh - joff;
   kxps = 0 > kxps ? 0 : kxps;
   kxps = kxp < kxps ? kxp : kxps;
   dnx = 6.28318530717959/(float) nx;
   dny = 6.28318530717959/(float) ny;
   zero = 0.0 + 0.0*_Complex_I;
   if (isign != 0)
      goto L30;
   if (kstrt > nxh) return;
/* prepare form factor array */
   for (j = 0; j < kxps; j++) {
      dkx = dnx*(float) (j + joff);
      jj = nyhd*j;
      at1 = dkx*dkx;
      at2 = pow((dkx*ax),2);
      for (k = 0; k < nyh; k++) {
         dky = dny*(float) k;
         at3 = dky*dky + at1;
         at4 = exp(-.5*(pow((dky*ay),2) + at2));
         if (at3==0.0) {
            ffc[k+jj] = affp + 1.0*_Complex_I;
         }
         else {
            ffc[k+jj] = (affp*at4/at3) + at4*_Complex_I;
         }
      }
   }
   return;
/* calculate force/charge and sum field energy */
L30: wp = 0.0;
   if (kstrt > nxh)
      goto L70;
/* mode numbers 0 < kx < nx/2 and 0 < ky < ny/2 */
   for (j = 0; j < kxps; j++) {
      dkx = dnx*(float) (j + joff);
      jj = nyhd*j;
      jk = nyv*j;
      if ((j+joff) > 0) {
         for (k = 1; k < nyh; k++) {
            k1 = ny - k;
            at1 = crealf(ffc[k+jj])*cimagf(ffc[k+jj]);
            at2 = dkx*at1;
            at3 = dny*at1*(float) k;
            zt1 = cimagf(q[k+jk]) - crealf(q[k+jk])*_Complex_I;
            zt2 = cimagf(q[k1+jk]) - crealf(q[k1+jk])*_Complex_I;
            fxy[3*k+3*jk] = at2*zt1;
            fxy[1+3*k+3*jk] = at3*zt1;
            fxy[2+3*k+3*jk] = zero;
            fxy[3*k1+3*jk] = at2*zt2;
            fxy[1+3*k1+3*jk] = -at3*zt2;
            fxy[2+3*k1+3*jk] = zero;
            wp += at1*(q[k+jk]*conjf(q[k+jk])
                  + q[k1+jk]*conjf(q[k1+jk]));
         }
/* mode numbers ky = 0, ny/2 */
         k1 = nyh;
         at1 = crealf(ffc[jj])*cimagf(ffc[jj]);
         at3 = dkx*at1;
         zt1 = cimagf(q[jk]) - crealf(q[jk])*_Complex_I;
         fxy[3*jk] = at3*zt1;
         fxy[1+3*jk] = zero;
         fxy[2+3*jk] = zero;
         fxy[3*k1+3*jk] = zero;
         fxy[1+3*k1+3*jk] = zero;
         fxy[2+3*k1+3*jk] = zero;
         wp += at1*(q[jk]*conjf(q[jk]));
      }
   }
/* mode numbers kx = 0, nx/2 */
   if (ks==0) {
      for (k = 1; k < nyh; k++) {
         k1 = ny - k;
         at1 = crealf(ffc[k])*cimagf(ffc[k]);
         at2 = dny*at1*(float) k;
         zt1 = cimagf(q[k]) - crealf(q[k])*_Complex_I;
         fxy[3*k] = zero;
         fxy[1+3*k] = at2*zt1;
         fxy[2+3*k] = zero;
         fxy[3*k1] = zero;
         fxy[1+3*k1] = zero;
         fxy[2+3*k1] = zero;
         wp += at1*(q[k]*conjf(q[k]));
      }
      k1 = 3*nyh;
      fxy[0] = zero;
      fxy[1] = zero;
      fxy[2] = zero;
      fxy[k1] = zero;
      fxy[1+k1] = zero;
      fxy[2+k1] = zero;
   }
L70:
   *we = wp*((float) nx)*((float) ny);
   return;
}

/*--------------------------------------------------------------------*/
void cppcuperp2(float complex cu[], int nx, int ny, int kstrt, int nyv,
                int kxp) {
/* this subroutine calculates the transverse current in fourier space
   input: all, output: cu
   approximate flop count is: 36*nxc*nyc
   and nxc*nyc divides
   where nxc = (nx/2-1)/nvp, nyc = ny/2 - 1, and nvp = number of procs
   the transverse current is calculated using the equation:
   cux[ky][kx] = cux(kx,ky)-kx*(kx*cux(kx,ky)+ky*cuy(kx,ky))/(kx*kx+ky*ky)
   cuy[ky][kx] = cuy(kx,ky)-ky*(kx*cux(kx,ky)+ky*cuy(kx,ky))/(kx*kx+ky*ky)
   where kx = 2pi*j/nx, ky = 2pi*k/ny, and j,k = fourier mode numbers,
   except for cux(kx=pi) = cuy(kx=pi) = 0, cux(ky=pi) = cuy(ky=pi) = 0,
   and cux(kx=0,ky=0) = cuy(kx=0,ky=0) = 0.
   cu[j][k][i] = i-th component of complex current density and
   for fourier mode (jj,k), where jj = j + kxp*(kstrt - 1)
   nx/ny = system length in x/y direction
   kstrt = starting data block number
   nyv = first dimension of field arrays, must be >= ny
   kxp = number of data values per block
local data                                                          */
   int nxh, nyh, ks, joff, kxps, j, jk, k, k1;
   float dnx, dny, dkx, dky, dkx2, at1;
   float complex zero, zt1;
   nxh = nx/2;
   nyh = 1 > ny/2 ? 1 : ny/2;
   ks = kstrt - 1;
   joff = kxp*ks;
   kxps = nxh - joff;
   kxps = 0 > kxps ? 0 : kxps;
   kxps = kxp < kxps ? kxp : kxps;
   dnx = 6.28318530717959/(float) nx;
   dny = 6.28318530717959/(float) ny;
   zero = 0.0 + 0.0*_Complex_I;
/* calculate transverse part of current */
   if (kstrt > nxh)
      return;
/* mode numbers 0 < kx < nx/2 and 0 < ky < ny/2 */
   for (j = 0; j < kxps; j++) {
      dkx = dnx*(float) (j + joff);
      dkx2 = dkx*dkx;
      jk = nyv*j;
      if ((j+joff) > 0) {
         for (k = 1; k < nyh; k++) {
            k1 = ny - k;
            dky = dny*(float) k;
            at1 = 1.0/(dky*dky + dkx2);
            zt1 = at1*(dkx*cu[3*k+3*jk] + dky*cu[1+3*k+3*jk]);
            cu[3*k+3*jk] -= dkx*zt1;
            cu[1+3*k+3*jk] -= dky*zt1;
            zt1 = at1*(dkx*cu[3*k1+3*jk] - dky*cu[1+3*k1+3*jk]);
            cu[3*k1+3*jk] -= dkx*zt1;
            cu[1+3*k1+3*jk] += dky*zt1;
         }
/* mode numbers ky = 0, ny/2 */
         k1 = nyh;
         cu[3*jk] = zero;
         cu[3*k1+3*jk] = zero;
         cu[1+3*k1+3*jk] = zero;
      }

   }
/* mode numbers kx = 0, nx/2 */
   if (ks==0) {
      for (k = 1; k < nyh; k++) {
         k1 = ny - k;
         cu[1+3*k] = zero;
         cu[3*k1] = zero;
         cu[1+3*k1] = zero;
      }
      k1 = 3*nyh;
      cu[0] = zero;
      cu[1] = zero;
      cu[k1] = zero;
      cu[1+k1] = zero;
   }
   return;
}

/*--------------------------------------------------------------------*/
void cppbbpoisp23(float complex cu[], float complex bxy[],
                  float complex ffc[], float ci, float *wm, int nx,
                  int ny, int kstrt, int nyv, int kxp, int nyhd) {
/* this subroutine solves 2-1/2d poisson's equation in fourier space for
   magnetic field (or convolution of magnetic field over particle shape)
   with periodic boundary conditions for distributed data.
   input: cu,ffc,ci,nx,ny,kstrt,nyv,kxp,nyhd, output: bxy,wm
   approximate flop count is: 85*nxc*nyc + 36*(nxc + nyc)
   where nxc = (nx/2-1)/nvp, nyc = ny/2 - 1, and nvp = number of procs
   magnetic field is calculated using the equations:
   bx[ky][kx] = ci*ci*sqrt(-1)*g[ky][kx]*ky*cuz[ky][kx]*s[ky][kx],
   by[ky][kx] = -ci*ci*sqrt(-1)*g[ky][kx]*kx*cuz[ky][kx]*s[ky][kx],
   bz[ky][kx] = ci*ci*sqrt(-1)*g[ky][kx]*(kx*cuy[ky][kx]-ky*cux[ky][kx])*
               s[ky][kx],
   where kx = 2pi*j/nx, ky = 2pi*k/ny, and j,k = fourier mode numbers,
   g[ky][kx] = (affp/(kx**2+ky**2))*s[ky][kx],
   s[ky][kx] = exp(-((kx*ax)**2+(ky*ay)**2)/2), except for
   bx(kx=pi) = by(kx=pi) = bz(kx=pi) = 0,
   bx(ky=pi) = by(ky=pi) = bz(ky=pi) = 0,
   bx(kx=0,ky=0) = by(kx=0,ky=0) = bz(kx=0,ky=0) = 0.
   cu[j][k][i] = i-th component of complex current density and
   bxy[j][k][i] = i-th component of complex magnetic field,
   for fourier mode (jj,k), where jj = j + kxp*(kstrt - 1)
   kxp = number of data values per block
   kstrt = starting data block number
   imag(ffc[j][k]) = finite-size particle shape factor s
   real(ffc[j][k]) = potential green's function g
   for fourier mode (jj,k), where jj = j + kxp*(kstrt - 1)
   ci = reciprocal of velocity of light
   magnetic field energy is also calculated, using
   wm = nx*ny*sum((affp/(kx**2+ky**2))*ci*ci
      |cu[ky][kx]*s[ky][kx]|**2), where
   affp = normalization constant = nx*ny/np, where np=number of particles
   this expression is valid only if the current is divergence-free
   nx/ny = system length in x/y direction
   nyv = second dimension of field arrays, must be >= ny
   nyhd = first dimension of form factor array, must be >= nyh
local data                                                 */
   int nxh, nyh, ks, joff, kxps, j, jj, jk, k, k1;
   float ci2, dnx, dny, dkx, dky, at1, at2, at3;
   float complex zero, zt1, zt2, zt3;
   double wp;
   nxh = nx/2;
   nyh = 1 > ny/2 ? 1 : ny/2;
   ks = kstrt - 1;
   joff = kxp*ks;
   kxps = nxh - joff;
   kxps = 0 > kxps ? 0 : kxps;
   kxps = kxp < kxps ? kxp : kxps;
   dnx = 6.28318530717959/(float) nx;
   dny = 6.28318530717959/(float) ny;
   zero = 0.0 + 0.0*_Complex_I;
   ci2 = ci*ci;
/* calculate magnetic field and sum field energy */
   wp = 0.0;
   if (kstrt > nxh)
      goto L40;
/* mode numbers 0 < kx < nx/2 and 0 < ky < ny/2 */
   for (j = 0; j < kxps; j++) {
      dkx = dnx*(float) (j + joff);
      jj = nyhd*j;
      jk = nyv*j;
      if ((j+joff) > 0) {
         for (k = 1; k < nyh; k++) {
            k1 = ny - k;
            dky = dny*(float) k;
            at1 = ci2*crealf(ffc[k+jj])*cimagf(ffc[k+jj]);
            at2 = dky*at1;
            at3 = dkx*at1;
            zt1 = -cimagf(cu[2+3*k+3*jk])
                + crealf(cu[2+3*k+3*jk])*_Complex_I;
            zt2 = -cimagf(cu[1+3*k+3*jk])
                + crealf(cu[1+3*k+3*jk])*_Complex_I;
            zt3 = -cimagf(cu[3*k+3*jk])
                + crealf(cu[3*k+3*jk])*_Complex_I;
            bxy[3*k+3*jk] = at2*zt1;
            bxy[1+3*k+3*jk] = -at3*zt1;
            bxy[2+3*k+3*jk] = at3*zt2 - at2*zt3;
            zt1 = -cimagf(cu[2+3*k1+3*jk])
                + crealf(cu[2+3*k1+3*jk])*_Complex_I;
            zt2 = -cimagf(cu[1+3*k1+3*jk])
                + crealf(cu[1+3*k1+3*jk])*_Complex_I;
            zt3 = -cimagf(cu[3*k1+3*jk])
                + crealf(cu[3*k1+3*jk])*_Complex_I;
            bxy[3*k1+3*jk] = -at2*zt1;
            bxy[1+3*k1+3*jk] = -at3*zt1;
            bxy[2+3*k1+3*jk] = at3*zt2 + at2*zt3;
            wp += at1*(cu[3*k+3*jk]*conjf(cu[3*k+3*jk])
               + cu[1+3*k+3*jk]*conjf(cu[1+3*k+3*jk])
               + cu[2+3*k+3*jk]*conjf(cu[2+3*k+3*jk])
               + cu[3*k1+3*jk]*conjf(cu[3*k1+3*jk])
               + cu[1+3*k1+3*jk]*conjf(cu[1+3*k1+3*jk])
               + cu[2+3*k1+3*jk]*conjf(cu[2+3*k1+3*jk]));
         }
/* mode numbers ky = 0, ny/2 */
         k1 = nyh;
         at1 = ci2*crealf(ffc[jj])*cimagf(ffc[jj]);
         at2 = dkx*at1;
         zt1 = -cimagf(cu[2+3*jk])
             + crealf(cu[2+3*jk])*_Complex_I;
         zt2 = -cimagf(cu[1+3*jk])
             + crealf(cu[1+3*jk])*_Complex_I;
         bxy[3*jk] = zero;
         bxy[1+3*jk] = -at2*zt1;
         bxy[2+3*jk] = at2*zt2;
         bxy[3*k1+3*jk] = zero;
         bxy[1+3*k1+3*jk] = zero;
         bxy[2+3*k1+3*jk] = zero;
         wp += at1*(cu[3*jk]*conjf(cu[3*jk])
            + cu[1+3*jk]*conjf(cu[1+3*jk])
            + cu[2+3*jk]*conjf(cu[2+3*jk]));
      }
   }
/* mode numbers kx = 0, nx/2 */
   if (ks==0) {
      for (k = 1; k < nyh; k++) {
         k1 = ny - k;
         dky = dny*(float) k;
         at1 = ci2*crealf(ffc[k])*cimagf(ffc[k]);
         at2 = dky*at1;
         zt1 = -cimagf(cu[2+3*k]) + crealf(cu[2+3*k])*_Complex_I;
         zt2 = -cimagf(cu[3*k]) + crealf(cu[3*k])*_Complex_I;
         bxy[3*k] = at2*zt1;
         bxy[1+3*k] = zero;
         bxy[2+3*k] = -at2*zt2;
         bxy[3*k1] = zero;
         bxy[1+3*k1] = zero;
         bxy[2+3*k1] = zero;
         wp += at1*(cu[3*k]*conjf(cu[3*k]) + cu[1+3*k]*conjf(cu[1+3*k])
            + cu[2+3*k]*conjf(cu[2+3*k]));
      }
      k1 = 3*nyh;
      bxy[0] = zero;
      bxy[1] = zero;
      bxy[2] = zero;
      bxy[k1] = zero;
      bxy[1+k1] = zero;
      bxy[2+k1] = zero;
   }
L40:
   *wm = wp*((float) nx)*((float) ny);
   return;
}

/*--------------------------------------------------------------------*/
void cppbaddext2(float bxy[], int nyp, float omx, float omy, float omz,
                 int nx, int nxe, int nypmx) {
/* adds constant to magnetic field for 2-1/2d code
   bxy = magnetic field
   nyp = number of primary (complete) gridpoints in particle partition
   omx/omy/omz = magnetic field electron cyclotron frequency in x/y/z
   nx = system length in x direction
   nxe = first dimension of field array, must be >= nx
   nypmx = maximum size of particle partition, including guard cells.
local data                                                 */
   int j, k;
   for (k = 0; k < nyp; k++) {
      for (j = 0; j < nx; j++) {
         bxy[3*j+3*nxe*k] += omx;
         bxy[1+3*j+3*nxe*k] += omy;
         bxy[2+3*j+3*nxe*k] += omz;
      }
   }
   return;
}

/*--------------------------------------------------------------------*/
void cppdcuperp23(float complex dcu[], float complex amu[], int nx,
                  int ny, int kstrt, int nyv, int kxp) {
/* this subroutine calculates transverse part of the derivative of
   the current density from the momentum flux
   in 2-1/2d with periodic boundary conditions.
   input: all, output: dcu
   approximate flop count is: 45*nxc*nyc
   and nxc*nyc divides
   where nxc = (nx/2-1)/nvp, nyc = ny/2 - 1, and nvp = number of procs
   the derivative of the current is calculated using the equations:
   dcu[kx][ky][0] = -sqrt(-1)*(kx*vx*vx+ky*vx*vy)
   dcu[kx][ky][1] = -sqrt(-1)*(kx*vx*vy+ky*vy*vy)
   dcu[kx][ky][2] = -sqrt(-1)*(kx*vx*vz+ky*vy*vz)
   where kx = 2pi*j/nx, ky = 2pi*k/ny, and j,k = fourier mode numbers,
   except for dcu(i,kx=pi) = dcu(i,ky=pi) = dcu(i,kx=0,ky=0) = 0.
   the transverse part is calculated using the equation:
   dcu[kx][ky][0] = dcu[kx][ky][0]-kx*(kx*dcu[kx][ky][0]
                  + ky*dcu[kx][ky][1])/(kx*kx+ky*ky)
   dcu[kx][ky][1] = dcu[kx][ky][1]-ky*(kx*dcu[kx][ky][0]
                  + ky*dcu[kx][ky][1])/(kx*kx+ky*ky)
   on output:
   dcu[j][k][i] = i-th component of transverse part of complex derivative
   of current for fourier mode (jj,k), where jj = j + kxp*(kstrt - 1)
   amu[j][k][0] = xx component of complex momentum flux
   amu[j][k][1] = xy component of complex momentum flux
   amu[j][k][2] = zx component of complex momentum flux
   amu[j][k][3] = zy component of complex momentum flux
   for fourier mode (jj,k), where jj = j + kxp*(kstrt - 1)
   nx/ny = system length in x/y direction
   kstrt = starting data block number
   nyv = second dimension of field arrays, must be >= ny
   kxp = number of data values per block
local data                                                 */
   int nxh, nyh, ks, joff, kxps, j, jk, k, k1;
   float dnx, dny, dkx, dky, dkx2, dky2, dkxy, dkxy2, at1;
   float complex zero, zt1, zt2, zt3;
   nxh = nx/2;
   nyh = 1 > ny/2 ? 1 : ny/2;
   ks = kstrt - 1;
   joff = kxp*ks;
   kxps = nxh - joff;
   kxps = 0 > kxps ? 0 : kxps;
   kxps = kxp < kxps ? kxp : kxps;
   dnx = 6.28318530717959/(float) nx;
   dny = 6.28318530717959/(float) ny;
   zero = 0.0 + 0.0*_Complex_I;
/* calculate transverse part of current */
   if (kstrt > nxh)
      return;
/* mode numbers 0 < kx < nx/2 and 0 < ky < ny/2 */
   for (j = 0; j < kxps; j++) {
      dkx = dnx*(float) (j + joff);
      dkx2 = dkx*dkx;
      jk = nyv*j;
      if ((j+joff) > 0) {
         for (k = 1; k < nyh; k++) {
            k1 = ny - k;
            dky = dny*(float) k;
            dky2 = dky*dky;
            dkxy = dkx*dky;
            dkxy2 = dky2 - dkx2;
            at1 = 1.0/(dkx2 + dky2);
            zt1 = cimagf(amu[4*k+4*jk])
                - crealf(amu[4*k+4*jk])*_Complex_I;
            zt2 = cimagf(amu[1+4*k+4*jk])
                - crealf(amu[1+4*k+4*jk])*_Complex_I;
            zt3 = at1*(dkxy*zt1 + dkxy2*zt2);
            dcu[3*k+3*jk] = dky*zt3;
            dcu[1+3*k+3*jk] = -dkx*zt3;
            zt1 = cimagf(amu[2+4*k+4*jk])
                - crealf(amu[2+4*k+4*jk])*_Complex_I;
            zt2 = cimagf(amu[3+4*k+4*jk])
                - crealf(amu[3+4*k+4*jk])*_Complex_I;
            dcu[2+3*k+3*jk] = dkx*zt1 + dky*zt2;
            zt1 = cimagf(amu[4*k1+4*jk])
                - crealf(amu[4*k1+4*jk])*_Complex_I;
            zt2 = cimagf(amu[1+4*k1+4*jk])
                - crealf(amu[1+4*k1+4*jk])*_Complex_I;
            zt3 = at1*(dkxy*zt1 - dkxy2*zt2);
            dcu[3*k1+3*jk] = dky*zt3;
            dcu[1+3*k1+3*jk] = dkx*zt3;
            zt1 = cimagf(amu[2+4*k1+4*jk])
                - crealf(amu[2+4*k1+4*jk])*_Complex_I;
            zt2 = cimagf(amu[3+4*k1+4*jk])
                - crealf(amu[3+4*k1+4*jk])*_Complex_I;
            dcu[2+3*k1+3*jk] = dkx*zt1 - dky*zt2;
         }
/* mode numbers ky = 0, ny/2 */
         k1 = nyh;
         zt2 = cimagf(amu[1+4*jk]) - crealf(amu[1+4*jk])*_Complex_I;
         dcu[3*jk] = zero;
         dcu[1+3*jk] = dkx*zt2;
         zt1 = cimagf(amu[2+4*jk]) - crealf(amu[2+4*jk])*_Complex_I;
         dcu[2+3*jk] = dkx*zt1;
         dcu[3*k1+3*jk] = zero;
         dcu[1+3*k1+3*jk] = zero;
         dcu[2+3*k1+3*jk] = zero;
      }
   }
/* mode numbers kx = 0, nx/2 */
   if (ks==0) {
      for (k = 1; k < nyh; k++) {
         k1 = ny - k;
         dky = dny*(float) k;
         zt2 = cimagf(amu[1+4*k]) - crealf(amu[1+4*k])*_Complex_I;
         dcu[3*k] = dky*zt2;
         dcu[1+3*k] = zero;
         zt2 = cimagf(amu[3+4*k]) - crealf(amu[3+4*k])*_Complex_I;
         dcu[2+3*k] = dky*zt2;
         dcu[3*k1] = zero;
         dcu[1+3*k1] = zero;
         dcu[2+3*k1] = zero;
      }
      k1 = 3*nyh;
      dcu[0] = zero;
      dcu[1] = zero;
      dcu[2] = zero;
      dcu[k1] = zero;
      dcu[1+k1] = zero;
      dcu[2+k1] = zero;
   }
   return;
}

/*--------------------------------------------------------------------*/
void cppadcuperp23(float complex dcu[], float complex amu[], int nx,
                   int ny, int kstrt, int nyv, int kxp) {
/* this subroutine calculates transverse part of the derivative of
   the current density from the momentum flux and acceleration density
   in 2-1/2d with periodic boundary conditions.
   input: all, output: dcu
   approximate flop count is: 65*nxc*nyc
   and nxc*nyc divides
   where nxc = (nx/2-1)/nvp, nyc = ny/2 - 1, and nvp = number of procs
   the derivative of the current is calculated using the equations:
   dcu[kx][ky][0] = dcu[kx][ky][0]-sqrt(-1)*(kx*vx*vx+ky*vx*vy)
   dcu[kx][ky][1] = dcu[kx][ky][1]-sqrt(-1)*(kx*vx*vy+ky*vy*vy)
   dcu[kx][ky][2] = dcu[kx][ky][2]-sqrt(-1)*(kx*vx*vz+ky*vy*vz)
   where kx = 2pi*j/nx, ky = 2pi*k/ny, and j,k = fourier mode numbers,
   except for dcu(i,kx=pi) = dcu(i,ky=pi) = dcu(i,kx=0,ky=0) = 0.
   the transverse part is calculated using the equation:
   dcu[kx][ky][0] = dcu[kx][ky][0]-kx*(kx*dcu[kx][ky][0]
                  + ky*dcu[kx][ky][1])/(kx*kx+ky*ky)
   dcu[kx][ky][1] = dcu[kx][ky][1]-ky*(kx*dcu[kx][ky][0]
                  + ky*dcu[kx][ky][1])/(kx*kx+ky*ky)
   on input:
   dcu[j][k][i] = complex acceleration density for fourier mode (jj,k1)
   on output:
   dcu[j][k][i] = i-th component of transverse part of complex derivative
   of current for fourier mode (jj,k1), where jj = j + kxp*(kstrt - 1)
   amu[j][k][0] = xx component of complex momentum flux
   amu[j][k][1] = xy component of complex momentum flux
   amu[j][k][2] = zx component of complex momentum flux
   amu[j][k][3] = zy component of complex momentum flux
   for fourier mode (jj,k), where jj = j + kxp*(kstrt - 1)
   nx/ny = system length in x/y direction
   kstrt = starting data block number
   nyv = second dimension of field arrays, must be >= ny
   kxp = number of data values per block
local data                                                 */
   int nxh, nyh, ks, joff, kxps, j, jk, k, k1;
   float dnx, dny, dkx, dky, dkx2, dky2, dkxy, dkxy2, at1;
   float complex zero, zt1, zt2, zt3;
   nxh = nx/2;
   nyh = 1 > ny/2 ? 1 : ny/2;
   ks = kstrt - 1;
   joff = kxp*ks;
   kxps = nxh - joff;
   kxps = 0 > kxps ? 0 : kxps;
   kxps = kxp < kxps ? kxp : kxps;
   dnx = 6.28318530717959/(float) nx;
   dny = 6.28318530717959/(float) ny;
   zero = 0.0 + 0.0*_Complex_I;
/* calculate transverse part of current */
   if (kstrt > nxh)
      return;
/* mode numbers 0 < kx < nx/2 and 0 < ky < ny/2 */
   for (j = 0; j < kxps; j++) {
      dkx = dnx*(float) (j + joff);
      dkx2 = dkx*dkx;
      jk = nyv*j;
      if ((j+joff) > 0) {
         for (k = 1; k < nyh; k++) {
            k1 = ny - k;
            dky = dny*(float) k;
            dky2 = dky*dky;
            dkxy = dkx*dky;
            dkxy2 = dky2 - dkx2;
            at1 = 1.0/(dkx2 + dky2);
            zt1 = cimagf(amu[4*k+4*jk])
                - crealf(amu[4*k+4*jk])*_Complex_I;
            zt2 = cimagf(amu[1+4*k+4*jk])
                - crealf(amu[1+4*k+4*jk])*_Complex_I;
            zt3 = at1*(dky*dcu[3*k+3*jk] - dkx*dcu[1+3*k+3*jk]
                + dkxy*zt1 + dkxy2*zt2);
            dcu[3*k+3*jk] = dky*zt3;
            dcu[1+3*k+3*jk] = -dkx*zt3;
            zt1 = cimagf(amu[2+4*k+4*jk])
                - crealf(amu[2+4*k+4*jk])*_Complex_I;
            zt2 = cimagf(amu[3+4*k+4*jk])
                - crealf(amu[3+4*k+4*jk])*_Complex_I;
            dcu[2+3*k+3*jk] += dkx*zt1 + dky*zt2;
            zt1 = cimagf(amu[4*k1+4*jk])
                - crealf(amu[4*k1+4*jk])*_Complex_I;
            zt2 = cimagf(amu[1+4*k1+4*jk])
                - crealf(amu[1+4*k1+4*jk])*_Complex_I;
            zt3 = at1*(dky*dcu[3*k1+3*jk] + dkx*dcu[1+3*k1+3*jk]
                + dkxy*zt1 - dkxy2*zt2);
            dcu[3*k1+3*jk] = dky*zt3;
            dcu[1+3*k1+3*jk] = dkx*zt3;
            zt1 = cimagf(amu[2+4*k1+4*jk])
                - crealf(amu[2+4*k1+4*jk])*_Complex_I;
            zt2 = cimagf(amu[3+4*k1+4*jk])
                - crealf(amu[3+4*k1+4*jk])*_Complex_I;
            dcu[2+3*k1+3*jk] += dkx*zt1 - dky*zt2;
         }
/* mode numbers ky = 0, ny/2 */
         k1 = nyh;
         zt2 = cimagf(amu[1+4*jk]) - crealf(amu[1+4*jk])*_Complex_I;
         dcu[3*jk] = zero;
         dcu[1+3*jk] += dkx*zt2;
         zt1 = cimagf(amu[2+4*jk]) - crealf(amu[2+4*jk])*_Complex_I;
         dcu[2+3*jk] += dkx*zt1;
         dcu[3*k1+3*jk] = zero;
         dcu[1+3*k1+3*jk] = zero;
         dcu[2+3*k1+3*jk] = zero;
      }
   }
/* mode numbers kx = 0, nx/2 */
   if (ks==0) {
      for (k = 1; k < nyh; k++) {
         k1 = ny - k;
         dky = dny*(float) k;
         zt2 = cimagf(amu[1+4*k]) - crealf(amu[1+4*k])*_Complex_I;
         dcu[3*k] += dky*zt2;
         dcu[1+3*k] = zero;
         zt2 = cimagf(amu[3+4*k]) - crealf(amu[3+4*k])*_Complex_I;
         dcu[2+3*k] += dky*zt2;
         dcu[3*k1] = zero;
         dcu[1+3*k1] = zero;
         dcu[2+3*k1] = zero;
      }
      k1 = 3*nyh;
      dcu[0] = zero;
      dcu[1] = zero;
      dcu[k1] = zero;
      dcu[1+k1] = zero;
      dcu[2+k1] = zero;
   }
   return;
}

/*--------------------------------------------------------------------*/
void cppepoisp23(float complex dcu[], float complex exy[], int isign,
                 float complex ffe[], float ax, float ay, float affp,
                 float wp0, float ci, float *wf, int nx, int ny,
                 int kstrt, int nyv, int kxp, int nyhd) {
/* this subroutine solves 2-1/2d poisson's equation in fourier space for
   transverse electric field (or convolution of transverse electric field
   over particle shape), with periodic boundary conditions.
   using algorithm described in J. Busnardo-Neto, P. L. Pritchett,
   A. T. Lin, and J. M. Dawson, J. Computational Phys. 23, 300 (1977).
   for isign = 0, input: isign,ax,ay,affp,wp0,nx,ny,kstrt,nyv,kxp,nyhd,
   output: ffe
   for isign /= 0, input: dcu,ffe,isign,affp,ci,nx,ny,kstrt,nyv,kxp,nyhd,
   output: exy,wf
   approximate flop count is: 59*nxc*nyc + 32*(nxc + nyc)
   where nxc = (nx/2-1)/nvp, nyc = ny/2 - 1, and nvp = number of procs
   if isign = 0, form factor array is prepared
   if isign = -1, smoothed transverse electric field is calculated
   using the equations:
   ex[ky][kx] = -ci*ci*g[ky][kx]*dcux[ky][kx]*s[ky][kx]
   ey[ky][kx] = -ci*ci*g[ky][kx]*dcuy[ky][kx])*s[ky][kx]
   ez[ky][kx] = -ci*ci*g[ky][kx]*dcuz[ky][kx]*s[ky][kx]
   where kx = 2pi*j/nx, ky = 2pi*k/ny, and j,k = fourier mode numbers,
   g[ky][kx] = (affp/(kx**2+ky**2))*s[ky][kx],
   s[ky][kx] = exp(-((kx*ax)**2+(ky*ay)**2)/2), except for
   ex(kx=pi) = ey(kx=pi) = ez(kx=pi) = 0,
   ex(ky=pi) = ey(ky=pi) = ez(ky=pi) = 0,
   ex(kx=0,ky=0) = ey(kx=0,ky=0) = ez(kx=0,ky=0) = 0.
   if isign = 1, unsmoothed transverse electric field is calculated
   using the equations:
   ex[ky][kx] = -ci*ci*g[ky][kx]*dcux[ky][kx]
   ey[ky][kx] = -ci*ci*g[ky][kx]*dcuy[ky][kx]
   ez[ky][kx] = -ci*ci*g[ky][kx]*dcuz[ky][kx]
   dcu[j][k][i] = i-th component of transverse part of complex derivative
   of current,
   exy[j][k][i] = i-th component of complex transverse electric field,
   for fourier mode (jj,k), where jj = j + kxp*(kstrt - 1)
   kxp = number of data values per block
   kstrt = starting data block number
   imag(ffe[j][k]) = finite-size particle shape factor s
   real(ffe[j][k]) = potential green's function g
   for fourier mode (jj,k), where jj = j + kxp*(kstrt - 1)
   ax/ay = half-width of particle in x/y direction
   affp = normalization constant = nx*ny/np, where np=number of particles
   wp0 = normalized total plasma frequency squared
   ci = reciprical of velocity of light
   transverse electric field energy is also calculated, using
   wf = nx*ny*sum((affp/((kx**2+ky**2)*ci*ci)**2)
      |dcu[ky][kx]*s[ky][kx]|**2)
   this expression is valid only if the derivative of current is
   divergence-free
   nx/ny = system length in x/y direction
   nyv = second dimension of field arrays, must be >= ny
   nyhd = first dimension of form factor array, must be >= nyh
local data                                                 */
   int nxh, nyh, ks, joff, kxps, j, jj, jk, k, k1;
   float dnx, dny, ci2, wpc, dkx, dky, at1, at2, at3, at4;
   float complex zero;
   double wp;
   nxh = nx/2;
   nyh = 1 > ny/2 ? 1 : ny/2;
   ks = kstrt - 1;
   joff = kxp*ks;
   kxps = nxh - joff;
   kxps = 0 > kxps ? 0 : kxps;
   kxps = kxp < kxps ? kxp : kxps;
   dnx = 6.28318530717959/(float) nx;
   dny = 6.28318530717959/(float) ny;
   zero = 0.0 + 0.0*_Complex_I;
   ci2 = ci*ci;
   if (isign != 0)
      goto L30;
   if (kstrt > nxh) return;
   wpc = wp0*ci2;
/* prepare form factor array */
   for (j = 0; j < kxps; j++) {
      dkx = dnx*(float) (j + joff);
      jj = nyhd*j;
      at1 = dkx*dkx;
      at2 = pow((dkx*ax),2);
      for (k = 0; k < nyh; k++) {
         dky = dny*(float) k;
         at3 = dky*dky + at1;
         at4 = exp(-.5*(pow((dky*ay),2) + at2));
         if (at3==0.0) {
            ffe[k+jj] = affp + 1.0*_Complex_I;
         }
         else {
            ffe[k+jj] = (affp*at4/(at3 + wpc*at4*at4)) + at4*_Complex_I;
         }
      }
   }
   return;
/* calculate smoothed transverse electric field and sum field energy */
L30: if (isign > 0)
      goto L80;
   wp = 0.0;
   if (kstrt > nxh)
      goto L70;
/* mode numbers 0 < kx < nx/2 and 0 < ky < ny/2 */
   for (j = 0; j < kxps; j++) {
      jj = nyhd*j;
      jk = nyv*j;
      if ((j+joff) > 0) {
         for (k = 1; k < nyh; k++) {
            k1 = ny - k;
            at2 = -ci2*crealf(ffe[k+jj]);
            at1 = at2*cimagf(ffe[k+jj]);
            at2 = at2*at2;
            exy[3*k+3*jk] = at1*dcu[3*k+3*jk];
            exy[1+3*k+3*jk] = at1*dcu[1+3*k+3*jk];
            exy[2+3*k+3*jk] = at1*dcu[2+3*k+3*jk];
            exy[3*k1+3*jk] = at1*dcu[3*k1+3*jk];
            exy[1+3*k1+3*jk] = at1*dcu[1+3*k1+3*jk];
            exy[2+3*k1+3*jk] = at1*dcu[2+3*k1+3*jk];
            wp += at2*(dcu[3*k+3*jk]*conjf(dcu[3*k+3*jk])
               + dcu[1+3*k+3*jk]*conjf(dcu[1+3*k+3*jk])
               + dcu[2+3*k+3*jk]*conjf(dcu[2+3*k+3*jk])
               + dcu[3*k1+3*jk]*conjf(dcu[3*k1+3*jk])
               + dcu[1+3*k1+3*jk]*conjf(dcu[1+3*k1+3*jk])
               + dcu[2+3*k1+3*jk]*conjf(dcu[2+3*k1+3*jk]));
         }
/* mode numbers ky = 0, ny/2 */
         k1 = nyh;
         at2 = -ci2*crealf(ffe[jj]);
         at1 = at2*cimagf(ffe[jj]);
         at2 = at2*at2;
         exy[3*jk] = at1*dcu[3*jk];
         exy[1+3*jk] = at1*dcu[1+3*jk];
         exy[2+3*jk] = at1*dcu[2+3*jk];
         exy[3*k1+3*jk] = zero;
         exy[1+3*k1+3*jk] = zero;
         exy[2+3*k1+3*jk] = zero;
         wp += at2*(dcu[3*jk]*conjf(dcu[3*jk])
            + dcu[1+3*jk]*conjf(dcu[1+3*jk])
            + dcu[2+3*jk]*conjf(dcu[2+3*jk]));
      }
   }
/* mode numbers kx = 0, nx/2 */
   if (ks==0) {
      for (k = 1; k < nyh; k++) {
         k1 = ny - k;
         at2 = -ci2*crealf(ffe[k]);
         at1 = at2*cimagf(ffe[k]);
         at2 = at2*at2;
         exy[3*k] = at1*dcu[3*k];
         exy[1+3*k] = at1*dcu[1+3*k];
         exy[2+3*k] = at1*dcu[2+3*k];
         exy[3*k1] = zero;
         exy[1+3*k1] = zero;
         exy[2+3*k1] = zero;
         wp += at2*(dcu[3*k]*conjf(dcu[3*k])
            + dcu[1+3*k]*conjf(dcu[1+3*k])
            + dcu[2+3*k]*conjf(dcu[2+3*k]));
      }
      k1 = 3*nyh;
      exy[0] = zero;
      exy[1] = zero;
      exy[2] = zero;
      exy[k1] = zero;
      exy[1+k1] = zero;
      exy[2+k1] = zero;
   }
L70:
   *wf = wp*((float) nx)*((float) ny)/affp;
   return;
/* calculate unsmoothed transverse electric field and sum field energy */
L80: wp = 0.0;
   if (kstrt > nxh)
      goto L120;
/* mode numbers 0 < kx < nx/2 and 0 < ky < ny/2*/
   for (j = 0; j < kxps; j++) {
      jj = nyhd*j;
      jk = nyv*j;
      if ((j+joff) > 0) {
         for (k = 1; k < nyh; k++) {
            k1 = ny - k;
            at2 = -ci2*crealf(ffe[k+jj]);
            at1 = at2*at2;
            exy[3*k+3*jk] = at2*dcu[3*k+3*jk];
            exy[1+3*k+3*jk] = at2*dcu[1+3*k+3*jk];
            exy[2+3*k+3*jk] = at2*dcu[2+3*k+3*jk];
            exy[3*k1+3*jk] = at2*dcu[3*k1+3*jk];
            exy[1+3*k1+3*jk] = at2*dcu[1+3*k1+3*jk];
            exy[2+3*k1+3*jk] = at2*dcu[2+3*k1+3*jk];
            wp += at1*(dcu[3*k+3*jk]*conjf(dcu[3*k+3*jk])
               + dcu[1+3*k+3*jk]*conjf(dcu[1+3*k+3*jk])
               + dcu[2+3*k+3*jk]*conjf(dcu[2+3*k+3*jk])
               + dcu[3*k1+3*jk]*conjf(dcu[3*k1+3*jk])
               + dcu[1+3*k1+3*jk]*conjf(dcu[1+3*k1+3*jk])
               + dcu[2+3*k1+3*jk]*conjf(dcu[2+3*k1+3*jk]));
         }
/* mode numbers ky = 0, ny/2 */
         k1 = nyh;
         at2 = -ci2*crealf(ffe[jj]);
         at1 = at2*at2;
         exy[3*jk] = at2*dcu[3*jk];
         exy[1+3*jk] = at2*dcu[1+3*jk];
         exy[2+3*jk] = at2*dcu[2+3*jk];
         exy[3*k1+3*jk] = zero;
         exy[1+3*k1+3*jk] = zero;
         exy[2+3*k1+3*jk] = zero;
         wp += at1*(dcu[3*jk]*conjf(dcu[3*jk])
            + dcu[1+3*jk]*conjf(dcu[1+3*jk])
            + dcu[2+3*jk]*conjf(dcu[2+3*jk]));
      }
   }
/* mode numbers kx = 0, nx/2 */
   if (ks==0) {
      for (k = 1; k < nyh; k++) {
         k1 = ny - k;
         at2 = -ci2*crealf(ffe[k]);
         at1 = at2*at2;
         exy[3*k] = at2*dcu[3*k];
         exy[1+3*k] = at2*dcu[1+3*k];
         exy[2+3*k] = at2*dcu[2+3*k];
         exy[3*k1] = zero;
         exy[1+3*k1] = zero;
         exy[2+3*k1] = zero;
         wp += at1*(dcu[3*k]*conjf(dcu[3*k])
            + dcu[1+3*k]*conjf(dcu[1+3*k])
            + dcu[2+3*k]*conjf(dcu[2+3*k]));
      }
      k1 = 3*nyh;
      exy[0] = zero;
      exy[1] = zero;
      exy[2] = zero;
      exy[k1] = zero;
      exy[1+k1] = zero;
      exy[2+k1] = zero;
   }
L120:
   *wf = wp*((float) nx)*((float) ny)/affp;
   return;
}

/*--------------------------------------------------------------------*/
void cppaddvrfield2(float a[], float b[], float c[], int ndim, int nxe,
                    int nypmx) {
/* this subroutine calculates a = b + c for distributed real vector field
local data                                                 */
   int i, j, k, nnxe;
   nnxe = ndim*nxe;
   for (k = 0; k < nypmx; k++) {
      for (j = 0; j < nxe; j++) {
         for (i = 0; i < ndim; i++) {
            a[i+ndim*j+nnxe*k] = b[i+ndim*j+nnxe*k]
                               + c[i+ndim*j+nnxe*k];
         }
      }
   }
   return;
}

/*--------------------------------------------------------------------*/
void cwpfft2rinit(int mixup[], float complex sct[], int indx, int indy,
                  int nxhyd, int nxyhd) {
/* this subroutine calculates tables needed by a two dimensional
   real to complex fast fourier transform and its inverse.
   input: indx, indy, nxhyd, nxyhd
   output: mixup, sct
   mixup = array of bit reversed addresses
   sct = sine/cosine table
   indx/indy = exponent which determines length in x/y direction,
   where nx=2**indx, ny=2**indy
   nxhyd = maximum of (nx/2,ny)
   nxyhd = one half of maximum of (nx,ny)
   written by viktor k. decyk, ucla
local data                                                            */
   int indx1, indx1y, nx, ny, nxy, nxhy, nxyh;
   int  j, k, lb, ll, jb, it;
   float dnxy, arg;
   indx1 = indx - 1;
   indx1y = indx1 > indy ? indx1 : indy;
   nx = 1L<<indx;
   ny = 1L<<indy;
   nxy = nx > ny ? nx : ny;
   nxhy = 1L<<indx1y;
/* bit-reverse index table: mixup[j] = 1 + reversed bits of j */
   for (j = 0; j < nxhy; j++) {
      lb = j;
      ll = 0;
      for (k = 0; k < indx1y; k++) {
         jb = lb/2;
         it = lb - 2*jb;
         lb = jb;
         ll = 2*ll + it;
      }
      mixup[j] = ll + 1;
   }
/* sine/cosine table for the angles 2*n*pi/nxy */
   nxyh = nxy/2;
   dnxy = 6.28318530717959/(float) nxy;
   for (j = 0; j < nxyh; j++) {
      arg = dnxy*(float) j;
      sct[j] = cosf(arg) - sinf(arg)*_Complex_I;
   }
   return;
}

/*--------------------------------------------------------------------*/
void cppfft2rxx(float complex f[], int isign, int mixup[],
                float complex sct[], int indx, int indy, int kstrt,
                int kypi, int kypp, int nxvh, int kypd, int nxhyd,
                int nxyhd) {
/* this subroutine performs the x part of a two dimensional real to
   complex fast fourier transform and its inverse, for a subset of y,
   using complex arithmetic, for data which is distributed in blocks
   for isign = (-1,1), input: all, output: f
   for isign = -1, approximate flop count: N*(5*log2(N) + 10)/nvp
   for isign = 1,  approximate flop count: N*(5*log2(N) + 8)/nvp
   where N = (nx/2)*ny, and nvp = number of procs
   indx/indy = exponent which determines length in x/y direction,
   where nx=2**indx, ny=2**indy
   if isign = -1, an inverse fourier transform is performed
   f[m][n] = (1/nx*ny)*sum(f[k][j]*exp(-sqrt(-1)*2pi*n*j/nx)
   if isign = 1, a forward fourier transform is performed
   f[k][j] = sum(f[m][n]*exp(sqrt(-1)*2pi*n*j/nx)
   kstrt = starting data block number
   kypi = initial y index used
   kypp = number of y indices used
   nxvh = first dimension of f
   kypd = second dimension of f
   mixup = array of bit reversed addresses
   sct = sine/cosine table
   nxhyd = maximum of (nx/2,ny)
   nxyhd = one half of maximum of (nx,ny)
   the real data is stored in a complex array of length nx/2, ny
   with the odd/even x points stored in the real/imaginary parts.
   in complex notation, fourier coefficients are stored as follows:
   f[k][j] = mode j,kk, where kk = k + kyp*(kstrt - 1)
   0 <= j < nx/2 and 0 <= kk < ny, except for
   f[k][0] = mode nx/2,kk, where ny/2+1 <= kk < ny, and
   imaginary part of f[0][0] = real part of mode nx/2,0 on mode kstrt=0
   imaginary part of f[0][0] = real part of mode nx/2,ny/2
   on mode kstrt=(ny/2)/kyp
   written by viktor k. decyk, ucla
   parallel, RISC optimized version
local data                                                            */
   int indx1, indx1y, nx, nxh, nxhh, ny;
   int nxy, nxhy, kypt, j, k, nrx;
   int i, m, ns, ns2, km, kmr, k1, k2, j1, j2, joff;
   float ani;
   float complex s, t, t1;
   indx1 = indx - 1;
   indx1y = indx1 > indy ? indx1 : indy;
   nx = 1L<<indx;
   nxh = nx/2;
   nxhh = nx/4;
   ny = 1L<<indy;
   nxy = nx > ny ? nx : ny;
   nxhy = 1L<<indx1y;
   kypt = kypi + kypp - 1;
   if (kstrt > ny)
      return;
   if (isign > 0)
      goto L100;
/* inverse fourier transform */
   ani = 0.5/(((float) nx)*((float) ny));
   nrx = nxhy/nxh;
/* bit-reverse array elements in x */
   for (j = 0; j < nxh; j++) {
      j1 = (mixup[j] - 1)/nrx;
      if (j >= j1)
         continue;
      for (k = kypi-1; k < kypt; k++) {
         joff = nxvh*k;
         t = f[j1+joff];
         f[j1+joff] = f[j+joff];
         f[j+joff] = t;
      }
   }
/* first transform in x */
   nrx = nxy/nxh;
   ns = 1;
   for (m = 0; m < indx1; m++) {
      ns2 = ns + ns;
      km = nxhh/ns;
      kmr = km*nrx;
      for (k = 0; k < km; k++) {
         k1 = ns2*k;
         k2 = k1 + ns;
         for (j = 0; j < ns; j++) {
            j1 = j + k1;
            j2 = j + k2;
            s = sct[kmr*j];
            for (i = kypi-1; i < kypt; i++) {
               joff = nxvh*i;
               t = s*f[j2+joff];
               f[j2+joff] = f[j1+joff] - t;
               f[j1+joff] += t;
            }
         }
      }
      ns = ns2;
   }
/* unscramble coefficients and normalize */
   kmr = nxy/nx;
   for (j = 1; j < nxhh; j++) {
      t1 = cimagf(sct[kmr*j]) - crealf(sct[kmr*j])*_Complex_I;
      for (k = kypi-1; k < kypt; k++) {
         joff = nxvh*k;
         t = conjf(f[nxh-j+joff]);
         s = f[j+joff] + t;
         t = (f[j+joff] - t)*t1;
         f[j+joff] = ani*(s + t);
         f[nxh-j+joff] = ani*conjf(s - t);
      }
   }
   ani = 2.0*ani;
   for (k = kypi-1; k < kypt; k++) {
      joff = nxvh*k;
      f[joff] = ani*((crealf(f[joff]) + cimagf(f[joff]))
                + (crealf(f[joff]) - cimagf(f[joff]))*_Complex_I);
      if (nxhh > 0)
         f[nxhh+joff] = ani*conjf(f[nxhh+joff]);
   }
   return;
/* forward fourier transform */
L100: kmr = nxy/nx;
/* scramble coefficients */
   for (j = 1; j < nxhh; j++) {
      t1 = cimagf(sct[kmr*j]) + crealf(sct[kmr*j])*_Complex_I;
      for (k = kypi-1; k < kypt; k++) {
         joff = nxvh*k;
         t = conjf(f[nxh-j+joff]);
         s = f[j+joff] + t;
         t = (f[j+joff] - t)*t1;
         f[j+joff] = s + t;
         f[nxh-j+joff] = conjf(s - t);
      }
   }
   for (k = kypi-1; k < kypt; k++) {
      joff = nxvh*k;
      f[joff] = (crealf(f[joff]) + cimagf(f[joff]))
                + (crealf(f[joff]) - cimagf(f[joff]))*_Complex_I;
      if (nxhh > 0)
         f[nxhh+joff] = 2.0*conjf(f[nxhh+joff]);
   }
   nrx = nxhy/nxh;
/* bit-reverse array elements in x */
   for (j = 0; j < nxh; j++) {
      j1 = (mixup[j] - 1)/nrx;
      if (j >= j1)
         continue;
      for (k = kypi-1; k < kypt; k++) {
         joff = nxvh*k;
         t = f[j1+joff];
         f[j1+joff] = f[j+joff];
         f[j+joff] = t;
      }
   }
/* then transform in x */
   nrx = nxy/nxh;
   ns = 1;
   for (m = 0; m < indx1; m++) {
      ns2 = ns + ns;
      km = nxhh/ns;
      kmr = km*nrx;
      for (k = 0; k < km; k++) {
         k1 = ns2*k;
         k2 = k1 + ns;
         for (j = 0; j < ns; j++) {
            j1 = j + k1;
            j2 = j + k2;
            s = conjf(sct[kmr*j]);
            for (i = kypi-1; i < kypt; i++) {
               joff = nxvh*i;
               t = s*f[j2+joff];
               f[j2+joff] = f[j1+joff] - t;
               f[j1+joff] += t;
            }
         }
      }
      ns = ns2;
   }
   return;
}

/*--------------------------------------------------------------------*/
void cppfft2rxy(float complex g[], int isign, int mixup[],
                float complex sct[], int indx, int indy, int kstrt,
                int kxpi, int kxpp, int nyv, int kxp, int nxhyd,
                int nxyhd) {
/* this subroutine performs the y part of a two dimensional real to
   complex fast fourier transform and its inverse, for a subset of x,
   using complex arithmetic, for data which is distributed in blocks
   for isign = (-1,1), input: all, output: g
   for isign = -1, approximate flop count: N*(5*log2(N) + 10)/nvp
   for isign = 1,  approximate flop count: N*(5*log2(N) + 8)/nvp
   where N = (nx/2)*ny, and nvp = number of procs
   indx/indy = exponent which determines length in x/y direction,
   where nx=2**indx, ny=2**indy
   if isign = -1, an inverse fourier transform is performed
   g[m][n] = sum(g[k][j]*exp(-sqrt(-1)*2pi*m*k/ny))
   if isign = 1, a forward fourier transform is performed
   g[k][j] = sum(g[m][n]*exp(sqrt(-1)*2pi*m*k/ny))
   kstrt = starting data block number
   kxp = number of x indices per block
   kxpi = initial x index used
   kxpp = number of x indices used
   nyv = first dimension of g
   kxp = number of data values per block in x
   mixup = array of bit reversed addresses
   sct = sine/cosine table
   nxhyd = maximum of (nx/2,ny)
   nxyhd = one half of maximum of (nx,ny)
   the real data is stored in a complex array of length nx/2, ny
   with the odd/even x points stored in the real/imaginary parts.
   in complex notation, fourier coefficients are stored as follows:
   g[k][j] = mode jj,k, where jj = j + kxp*(kstrt - 1)
   0 <= jj < nx/2 and 0 <= k < ny, except for
   g[0][k] = mode nx/2,k, where ny/2+1 <= k < ny, and
   imaginary part of g[0][0] = real part of mode nx/2,0 and
   imaginary part of g[1][ny/2] = real part of mode nx/2,ny/2
   on node kstrt=0
   written by viktor k. decyk, ucla
   parallel, RISC optimized version
local data                                                            */
   int indx1, indx1y, nx, nxh, ny, nyh;
   int nxy, nxhy, ks, kxpt, j, k, nry;
   int i, m, ns, ns2, km, kmr, k1, k2, j1, j2, koff;
   float complex s, t;
   indx1 = indx - 1;
   indx1y = indx1 > indy ? indx1 : indy;
   nx = 1L<<indx;
   nxh = nx/2;
   ny = 1L<<indy;
   nyh = ny/2;
   nxy = nx > ny ? nx : ny;
   nxhy = 1L<<indx1y;
   ks = kstrt - 1;
   kxpt = kxpi + kxpp - 1;
   if (kstrt > nxh)
      return;
   if (isign > 0)
      goto L80;
/* inverse fourier transform */
   nry = nxhy/ny;
/* bit-reverse array elements in y */
   for (k = 0; k < ny; k++) {
      k1 = (mixup[k] - 1)/nry;
      if (k >= k1)
         continue;
      for (j = kxpi-1; j < kxpt; j++) {
         koff = nyv*j;
         t = g[k1+koff];
         g[k1+koff] = g[k+koff];
         g[k+koff] = t;
      }
   }
/* then transform in y */
   nry = nxy/ny;
   ns = 1;
   for (m = 0; m < indy; m++) {
      ns2 = ns + ns;
      km = nyh/ns;
      kmr = km*nry;
      for (k = 0; k < km; k++) {
         k1 = ns2*k;
         k2 = k1 + ns;
         for (j = 0; j < ns; j++) {
            j1 = j + k1;
            j2 = j + k2;
            s = sct[kmr*j];
            for (i = kxpi-1; i < kxpt; i++) {
               koff = nyv*i;
               t = s*g[j2+koff];
               g[j2+koff] = g[j1+koff] - t;
               g[j1+koff] += t;
            }
         }
      }
      ns = ns2;
   }
/* unscramble modes kx = 0, nx/2 */
   if (ks > 0)
      return;
   for (k = 1; k < nyh; k++) {
      if (kxpi==1) {
         s = g[ny-k];
         g[ny-k] = 0.5*(cimagf(g[k] + s) + crealf(g[k] - s)*_Complex_I);
         g[k] = 0.5*(crealf(g[k] + s) + cimagf(g[k] - s)*_Complex_I);
      }
   }
   return;
/* forward fourier transform */
/* scramble modes kx = 0, nx/2 */
L80: if (ks==0) {
      for (k = 1; k < nyh; k++) {
         if (kxpi==1) {
            s = cimagf(g[ny-k]) + crealf(g[ny-k])*_Complex_I;
            g[ny-k] = conjf(g[k] - s);
            g[k] += s;
         }
      }
   }
   nry = nxhy/ny;
/* bit-reverse array elements in y */
   for (k = 0; k < ny; k++) {
      k1 = (mixup[k] - 1)/nry;
      if (k >= k1)
         continue;
      for (j = kxpi-1; j < kxpt; j++) {
         koff = nyv*j;
         t = g[k1+koff];
         g[k1+koff] = g[k+koff];
         g[k+koff] = t;
      }
   }
/* first transform in y */
   nry = nxy/ny;
   ns = 1;
   for (m = 0; m < indy; m++) {
      ns2 = ns + ns;
      km = nyh/ns;
      kmr = km*nry;
      for (k = 0; k < km; k++) {
         k1 = ns2*k;
         k2 = k1 + ns;
         for (j = 0; j < ns; j++) {
            j1 = j + k1;
            j2 = j + k2;
            s = conjf(sct[kmr*j]);
            for (i = kxpi-1; i < kxpt; i++) {
               koff = nyv*i;
               t = s*g[j2+koff];
               g[j2+koff] = g[j1+koff] - t;
               g[j1+koff] += t;
            }
         }
      }
      ns = ns2;
   }
   return;
}

/*--------------------------------------------------------------------*/
void cppfft2r3xx(float complex f[], int isign, int mixup[],
                 float complex sct[], int indx, int indy, int kstrt,
                 int kypi, int kypp, int nxvh, int kypd, int nxhyd,
                 int nxyhd) {
/* this subroutine performs the x part of 3 two dimensional real to
   complex fast fourier transforms and their inverses, for a subset of y,
   using complex arithmetic, for data which is distributed in blocks
   for isign = (-1,1), input: all, output: f
   for isign = -1, approximate flop count: N*(5*log2(N) + 10)/nvp
   for isign = 1,  approximate flop count: N*(5*log2(N) + 8)/nvp
   where N = (nx/2)*ny, and nvp = number of procs
   indx/indy = exponent which determines length in x/y direction,
   where nx=2**indx, ny=2**indy
   if isign = -1, an inverse fourier transform is performed
   f[m][n][0:2] = (1/nx*ny)*sum(f[k][j][0:2]*exp(-sqrt(-1)*2pi*n*j/nx)
   if isign = 1, a forward fourier transform is performed
   f[k][j][0:2] = sum(f[m][n][0:2]*exp(sqrt(-1)*2pi*n*j/nx)*
   kstrt = starting data block number
   kypi = initial y index used
   kypp = number of y indices used
   nxvh = first dimension of f
   kypd = second dimension of f
   mixup = array of bit reversed addresses
   sct = sine/cosine table
   nxhyd = maximum of (nx/2,ny)
   nxyhd = one half of maximum of (nx,ny)
   the real data is stored in a complex array of length nx/2, ny
   with the odd/even x points stored in the real/imaginary parts.
   in complex notation, fourier coefficients are stored as follows:
   f[k][j][0:2] = mode j,kk, where kk = k + kyp*(kstrt - 1)
   0 <= j < nx/2 and 0 <= kk < ny, except for
   f[k][0][0:2] = mode nx/2,kk, where ny/2+1 <= kk < ny, and
   imaginary part of f[0][0][0:2] = real part of mode nx/2,0
   on mode kstrt=0
   imaginary part of f[0][0][0:2] = real part of mode nx/2,ny/2
   on mode kstrt=(ny/2)/kyp
   written by viktor k. decyk, ucla
   parallel, RISC optimized version
local data                                                            */
   int indx1, indx1y, nx, nxh, nxhh, ny;
   int nxy, nxhy, kypt, j, k, nrx;
   int i, m, ns, ns2, km, kmr, k1, k2, j1, j2, joff;
   float ani, at1, at2;
   float complex s, t, t1, t2, t3;
   indx1 = indx - 1;
   indx1y = indx1 > indy ? indx1 : indy;
   nx = 1L<<indx;
   nxh = nx/2;
   nxhh = nx/4;
   ny = 1L<<indy;
   nxy = nx > ny ? nx : ny;
   nxhy = 1L<<indx1y;
   kypt = kypi + kypp - 1;
   if (kstrt > ny)
      return;
   if (isign > 0)
      goto L140;
/* inverse fourier transform */
   ani = 0.5/(((float) nx)*((float) ny));
   nrx = nxhy/nxh;
/* swap complex components */
   for (k = kypi-1; k < kypt; k++) {
      joff = 3*nxvh*k;
      for (j = 0; j < nxh; j++) {
         at1 = crealf(f[2+3*j+joff]);
         f[2+3*j+joff] = crealf(f[1+3*j+joff])
                       + cimagf(f[2+3*j+joff])*_Complex_I;
         at2 = cimagf(f[1+3*j+joff]);
         f[1+3*j+joff] = cimagf(f[3*j+joff]) + at1*_Complex_I;
         f[3*j+joff] = crealf(f[3*j+joff]) + at2*_Complex_I;
       }
   }
/* bit-reverse array elements in x */
   for (j = 0; j < nxh; j++) {
      j1 = (mixup[j] - 1)/nrx;
      if (j >= j1)
         continue;
      for (k = kypi-1; k < kypt; k++) {
         joff = 3*nxvh*k;
         t1 = f[3*j1+joff];
         t2 = f[1+3*j1+joff];
         t3 = f[2+3*j1+joff];
         f[3*j1+joff] = f[3*j+joff];
         f[1+3*j1+joff] = f[1+3*j+joff];
         f[2+3*j1+joff] = f[2+3*j+joff];
         f[3*j+joff] = t1;
         f[1+3*j+joff] = t2;
         f[2+3*j+joff] = t3;
      }
   }
/* first transform in x */
   nrx = nxy/nxh;
   ns = 1;
   for (m = 0; m < indx1; m++) {
      ns2 = ns + ns;
      km = nxhh/ns;
      kmr = km*nrx;
      for (k = 0; k < km; k++) {
         k1 = ns2*k;
         k2 = k1 + ns;
         for (j = 0; j < ns; j++) {
            j1 = j + k1;
            j2 = j + k2;
            s = sct[kmr*j];
            for (i = kypi-1; i < kypt; i++) {
               joff = 3*nxvh*i;
               t1 = s*f[3*j2+joff];
               t2 = s*f[1+3*j2+joff];
               t3 = s*f[2+3*j2+joff];
               f[3*j2+joff] = f[3*j1+joff] - t1;
               f[1+3*j2+joff] = f[1+3*j1+joff] - t2;
               f[2+3*j2+joff] = f[2+3*j1+joff] - t3;
               f[3*j1+joff] += t1;
               f[1+3*j1+joff] += t2;
               f[2+3*j1+joff] += t3;
            }
         }
      }
      ns = ns2;
   }
/* unscramble coefficients and normalize */
   kmr = nxy/nx;
   for (j = 1; j < nxhh; j++) {
      t1 = cimagf(sct[kmr*j]) - crealf(sct[kmr*j])*_Complex_I;
      for (k = kypi-1; k < kypt; k++) {
         joff = 3*nxvh*k;
         for (i = 0; i < 3; i++) {
            t = conjf(f[i+3*(nxh-j)+joff]);
            s = f[i+3*j+joff] + t;
            t = (f[i+3*j+joff] - t)*t1;
            f[i+3*j+joff] = ani*(s + t);
            f[i+3*(nxh-j)+joff] = ani*conjf(s - t);
         }
      }
   }
   ani = 2.0*ani;
   for (k = kypi-1; k < kypt; k++) {
      joff = 3*nxvh*k;
      for (i = 0; i < 3; i++) {
         f[i+joff] = ani*((crealf(f[i+joff]) + cimagf(f[i+joff]))
                      + (crealf(f[i+joff]) - cimagf(f[i+joff]))*_Complex_I);
         if (nxhh > 0)
            f[i+3*nxhh+joff] = ani*conjf(f[i+3*nxhh+joff]);
      }
   }
   return;
/* forward fourier transform */
L140: kmr = nxy/nx;
/* scramble coefficients */
   for (j = 1; j < nxhh; j++) {
      t1 = cimagf(sct[kmr*j]) + crealf(sct[kmr*j])*_Complex_I;
      for (k = kypi-1; k < kypt; k++) {
         joff = 3*nxvh*k;
         for (i = 0; i < 3; i++) {
            t = conjf(f[i+3*(nxh-j)+joff]);
            s = f[i+3*j+joff] + t;
            t = (f[i+3*j+joff] - t)*t1;
            f[i+3*j+joff] = s + t;
            f[i+3*(nxh-j)+joff] = conjf(s - t);
         }
      }
   }
   for (k = kypi-1; k < kypt; k++) {
      joff = 3*nxvh*k;
      for (i = 0; i < 3; i++) {
         f[i+joff] = (crealf(f[i+joff]) + cimagf(f[i+joff]))
                      + (crealf(f[i+joff]) - cimagf(f[i+joff]))*_Complex_I;
         if (nxhh > 0)
            f[i+3*nxhh+joff] = 2.0*conjf(f[i+3*nxhh+joff]);
      }
   }
   nrx = nxhy/nxh;
/* bit-reverse array elements in x */
   for (j = 0; j < nxh; j++) {
      j1 = (mixup[j] - 1)/nrx;
      if (j >= j1)
         continue;
      for (k = kypi-1; k < kypt; k++) {
         joff = 3*nxvh*k;
         t1 = f[3*j1+joff];
         t2 = f[1+3*j1+joff];
         t3 = f[2+3*j1+joff];
         f[3*j1+joff] = f[3*j+joff];
         f[1+3*j1+joff] = f[1+3*j+joff];
         f[2+3*j1+joff] = f[2+3*j+joff];
         f[3*j+joff] = t1;
         f[1+3*j+joff] = t2;
         f[2+3*j+joff] = t3;
      }
   }
/* then transform in x */
   nrx = nxy/nxh;
   ns = 1;
   for (m = 0; m < indx1; m++) {
      ns2 = ns + ns;
      km = nxhh/ns;
      kmr = km*nrx;
      for (k = 0; k < km; k++) {
         k1 = ns2*k;
         k2 = k1 + ns;
         for (j = 0; j < ns; j++) {
            j1 = j + k1;
            j2 = j + k2;
            s = conjf(sct[kmr*j]);
            for (i = kypi-1; i < kypt; i++) {
               joff = 3*nxvh*i;
               t1 = s*f[3*j2+joff];
               t2 = s*f[1+3*j2+joff];
               t3 = s*f[2+3*j2+joff];
               f[3*j2+joff] = f[3*j1+joff] - t1;
               f[1+3*j2+joff] = f[1+3*j1+joff] - t2;
               f[2+3*j2+joff] = f[2+3*j1+joff] - t3;
               f[3*j1+joff] += t1;
               f[1+3*j1+joff] +=  t2;
               f[2+3*j1+joff] +=  t3;
            }
         }
      }
      ns = ns2;
   }
/* swap complex components */
   for (k = kypi-1; k < kypt; k++) {
      joff = 3*nxvh*k;
      for (j = 0; j < nxh; j++) {
         at1 = crealf(f[2+3*j+joff]);
         f[2+3*j+joff] = cimagf(f[1+3*j+joff])
                       + cimagf(f[2+3*j+joff])*_Complex_I;
         at2 = crealf(f[1+3*j+joff]);
         f[1+3*j+joff] = at1 + cimagf(f[3*j+joff])*_Complex_I;
         f[3*j+joff] = crealf(f[3*j+joff]) + at2*_Complex_I;
      }
   }
   return;
}

/*--------------------------------------------------------------------*/
void cppfft2r3xy(float complex g[], int isign, int mixup[],
                 float complex sct[], int indx, int indy, int kstrt,
                 int kxpi, int kxpp, int nyv, int kxp, int nxhyd,
                 int nxyhd) {
/* this subroutine performs the y part of 3 two dimensional real to
   complex fast fourier transforms and their inverses, for a subset of x,
   using complex arithmetic, for data which is distributed in blocks
   for isign = (-1,1), input: all, output: g
   for isign = -1, approximate flop count: N*(5*log2(N) + 10)/nvp
   for isign = 1,  approximate flop count: N*(5*log2(N) + 8)/nvp
   where N = (nx/2)*ny, and nvp = number of procs
   indx/indy = exponent which determines length in x/y direction,
   where nx=2**indx, ny=2**indy
   if isign = -1, an inverse fourier transform is performed
   g[n][m][0:2] = sum(g[j][k][0:2]*exp(-sqrt(-1)*2pi*m*k/ny))
   if isign = 1, a forward fourier transform is performed
   g[j][k][0:2] = sum(g[n][m][0:2]*exp(sqrt(-1)*2pi*m*k/ny))
   kstrt = starting data block number
   kxpi = initial x index used
   kxpp = number of x indices used
   nyv = first dimension of g
   kxp = number of data values per block in x
   mixup = array of bit reversed addresses
   sct = sine/cosine table
   nxhyd = maximum of (nx/2,ny)
   nxyhd = one half of maximum of (nx,ny)
   the real data is stored in a complex array of length nx/2, ny
   with the odd/even x points stored in the real/imaginary parts.
   in complex notation, fourier coefficients are stored as follows:
   g[j][k][0:2] = mode jj,k, where jj = j + kxp*(kstrt - 1)
   0 <= jj < nx/2 and 0 <= k < ny, except for
   g[0][k][0:2] = mode nx/2,k, where ny/2+1 <= k < ny, and
   imaginary part of g[0][0][0:2] = real part of mode nx/2,0 and
   imaginary part of g[0][ny/2][0:2] = real part of mode nx/2,ny/2
   on node kstrt=0
   written by viktor k. decyk, ucla
   parallel, RISC optimized version
local data                                                            */
   int indx1, indx1y, nx, nxh, ny, nyh;
   int nxy, nxhy, ks, kxpt, j, k, nry;
   int i, m, ns, ns2, km, kmr, k1, k2, j1, j2, koff;
   float complex s, t1, t2, t3;
   indx1 = indx - 1;
   indx1y = indx1 > indy ? indx1 : indy;
   nx = 1L<<indx;
   nxh = nx/2;
   ny = 1L<<indy;
   nyh = ny/2;
   nxy = nx > ny ? nx : ny;
   nxhy = 1L<<indx1y;
   ks = kstrt - 1;
   kxpt = kxpi + kxpp - 1;
   if (kstrt > nxh)
      return;
   if (isign > 0)
      goto L90;
/* inverse fourier transform */
   nry = nxhy/ny;
/* bit-reverse array elements in y */
   for (k = 0; k < ny; k++) {
      k1 = (mixup[k] - 1)/nry;
      if (k >= k1)
         continue;
      for (j = kxpi-1; j < kxpt; j++) {
         koff = 3*nyv*j;
         t1 = g[3*k1+koff];
         t2 = g[1+3*k1+koff];
         t3 = g[2+3*k1+koff];
         g[3*k1+koff] = g[3*k+koff];
         g[1+3*k1+koff] = g[1+3*k+koff];
         g[2+3*k1+koff] = g[2+3*k+koff];
         g[3*k+koff] = t1;
         g[1+3*k+koff] = t2;
         g[2+3*k+koff] = t3;
      }
   }
/* then transform in y */
   nry = nxy/ny;
   ns = 1;
   for (m = 0; m < indy; m++) {
      ns2 = ns + ns;
      km = nyh/ns;
      kmr = km*nry;
      for (k = 0; k < km; k++) {
         k1 = ns2*k;
         k2 = k1 + ns;
         for (j = 0; j < ns; j++) {
            j1 = j + k1;
            j2 = j + k2;
            s = sct[kmr*j];
            for (i = kxpi-1; i < kxpt; i++) {
               koff = 3*nyv*i;
               t1 = s*g[3*j2+koff];
               t2 = s*g[1+3*j2+koff];
               t3 = s*g[2+3*j2+koff];
               g[3*j2+koff] = g[3*j1+koff] - t1;
               g[1+3*j2+koff] = g[1+3*j1+koff] - t2;
               g[2+3*j2+koff] = g[2+3*j1+koff] - t3;
               g[3*j1+koff] += t1;
               g[1+3*j1+koff] += t2;
               g[2+3*j1+koff] += t3;
            }
         }
      }
      ns = ns2;
   }
/* unscramble modes kx = 0, nx/2 */
   if (ks > 0)
      return;
   for (k = 1; k < nyh; k++) {
      if (kxpi==1) {
         for (i = 0; i < 3; i++) {
            s = g[i+3*(ny-k)];
            g[i+3*(ny-k)] = 0.5*(cimagf(g[i+3*k] + s)
                             + crealf(g[i+3*k] - s)*_Complex_I);
            g[i+3*k] = 0.5*(crealf(g[i+3*k] + s)
                             + cimagf(g[i+3*k] - s)*_Complex_I);
         }
      }
   }
   return;
/* forward fourier transform */
/* scramble modes kx = 0, nx/2 */
L90: if (ks==0) {
      for (k = 1; k < nyh; k++) {
         if (kxpi==1) {
            for (i = 0; i < 3; i++) {
               s = cimagf(g[i+3*(ny-k)])
                   + crealf(g[i+3*(ny-k)])*_Complex_I;
               g[i+3*(ny-k)] = conjf(g[i+3*k] - s);
               g[i+3*k] += s;
            }
         }
      }
   }
   nry = nxhy/ny;
/* bit-reverse array elements in y */
   for (k = 0; k < ny; k++) {
      k1 = (mixup[k] - 1)/nry;
      if (k >= k1)
         continue;
      for (j = kxpi-1; j < kxpt; j++) {
         koff = 3*nyv*j;
         t1 = g[3*k1+koff];
         t2 = g[1+3*k1+koff];
         t3 = g[2+3*k1+koff];
         g[3*k1+koff] = g[3*k+koff];
         g[1+3*k1+koff] = g[1+3*k+koff];
         g[2+3*k1+koff] = g[2+3*k+koff];
         g[3*k+koff] = t1;
         g[1+3*k+koff] = t2;
         g[2+3*k+koff] = t3;
      }
   }
/* first transform in y */
   nry = nxy/ny;
   ns = 1;
   for (m = 0; m < indy; m++) {
      ns2 = ns + ns;
      km = nyh/ns;
      kmr = km*nry;
      for (k = 0; k < km; k++) {
         k1 = ns2*k;
         k2 = k1 + ns;
         for (j = 0; j < ns; j++) {
            j1 = j + k1;
            j2 = j + k2;
            s = conjf(sct[kmr*j]);
            for (i = kxpi-1; i < kxpt; i++) {
               koff = 3*nyv*i;
               t1 = s*g[3*j2+koff];
               t2 = s*g[1+3*j2+koff];
               t3 = s*g[2+3*j2+koff];
               g[3*j2+koff] = g[3*j1+koff] - t1;
               g[1+3*j2+koff] = g[1+3*j1+koff] - t2;
               g[2+3*j2+koff] = g[2+3*j1+koff] - t3;
               g[3*j1+koff] += t1;
               g[1+3*j1+koff] += t2;
               g[2+3*j1+koff] += t3;
            }
         }
      }
      ns = ns2;
   }
   return;
}

/*--------------------------------------------------------------------*/
void cppfft2rnxx(float complex f[], float complex ss[], int isign,
                 int mixup[], float complex sct[], int indx, int indy,
                 int kstrt, int kypi, int kypp, int nxvh, int kypd,
                 int ndim, int nxhyd, int nxyhd) {
/* this subroutine performs the x part of N two dimensional real to
   complex fast fourier transforms and their inverses, for a subset of y,
   using complex arithmetic, where N = ndim
   for data which is distributed in blocks
   for isign = (-1,1), input: all, output: f
   for isign = -1, approximate flop count: M*(5*log2(N) + 10)/nvp
   for isign = 1,  approximate flop count: M*(5*log2(N) + 8)/nvp
   where M = (nx/2)*ny, and nvp = number of procs
   indx/indy = exponent which determines length in x/y direction,
   where nx=2**indx, ny=2**indy
   if isign = -1, an inverse fourier transform is performed
   f[m][n][0:N-1] = (1/nx*ny)*sum(f[k][j][0:N-1]*exp(-sqrt(-1)*2pi*n*j/nx)
   if isign = 1, a forward fourier transform is performed
   f[k][j][0:N-1] = sum(f[m][n][0:N-1]*exp(sqrt(-1)*2pi*n*j/nx)*
   kstrt = starting data block number
   kypi = initial y index used
   kypp = number of y indices used
   nxvh = first dimension of f
   kypd = second dimension of f
   ndim = leading dimension of arrays f and g
   ss = scratch array
   mixup = array of bit reversed addresses
   sct = sine/cosine table
   nxhyd = maximum of (nx/2,ny)
   nxyhd = one half of maximum of (nx,ny)
   the real data is stored in a complex array of length nx/2, ny
   with the odd/even x points stored in the real/imaginary parts.
   in complex notation, fourier coefficients are stored as follows:
   f[k][j][0:N-1] = mode j,kk, where kk = k + kyp*(kstrt - 1)
   0 <= j < nx/2 and 0 <= kk < ny, except for
   f[k][0][0:N-1] = mode nx/2,kk, where ny/2+1 <= kk < ny, and
   imaginary part of f[0][0][0:N-1] = real part of mode nx/2,0
   on mode kstrt=0
   imaginary part of f[0][0][0:N-1] = real part of mode nx/2,ny/2
   on mode kstrt=(ny/2)/kyp
   written by viktor k. decyk, ucla
   parallel, RISC optimized version
local data                                                            */
   int indx1, indx1y, nx, nxh, nxhh, ny;
   int nxy, nxhy, kypt, j, k, nrx;
   int i, m, ns, ns2, km, kmr, k1, k2, j1, j2, jj, joff;
   float ani;
   float complex s, t, t1;
   indx1 = indx - 1;
   indx1y = indx1 > indy ? indx1 : indy;
   nx = 1L<<indx;
   nxh = nx/2;
   nxhh = nx/4;
   ny = 1L<<indy;
   nxy = nx > ny ? nx : ny;
   nxhy = 1L<<indx1y;
   kypt = kypi + kypp - 1;
   if (kstrt > ny)
      return;
   if (isign > 0)
      goto L140;
/* inverse fourier transform */
   ani = 0.5/(((float) nx)*((float) ny));
   nrx = nxhy/nxh;
/* swap complex components */
   cppswapc2n((float *)f,(float *)ss,isign,nxh,kypi,kypt,nxvh,kypd,
              ndim);
/* bit-reverse array elements in x */
   for (j = 0; j < nxh; j++) {
      j1 = (mixup[j] - 1)/nrx;
      if (j >= j1)
         continue;
      for (k = kypi-1; k < kypt; k++) {
         joff = ndim*nxvh*k;
         for (jj = 0; jj < ndim; jj++) {
            t1 = f[jj+ndim*j1+joff];
            f[jj+ndim*j1+joff] = f[jj+ndim*j+joff];
            f[jj+ndim*j+joff] = t1;
         }
      }
   }
/* first transform in x */
   nrx = nxy/nxh;
   ns = 1;
   for (m = 0; m < indx1; m++) {
      ns2 = ns + ns;
      km = nxhh/ns;
      kmr = km*nrx;
      for (k = 0; k < km; k++) {
         k1 = ns2*k;
         k2 = k1 + ns;
         for (j = 0; j < ns; j++) {
            j1 = j + k1;
            j2 = j + k2;
            s = sct[kmr*j];
            for (i = kypi-1; i < kypt; i++) {
               joff = ndim*nxvh*i;
               for (jj = 0; jj < ndim; jj++) {
                  t1 = s*f[jj+ndim*j2+joff];
                  f[jj+ndim*j2+joff] = f[jj+ndim*j1+joff] - t1;
                  f[jj+ndim*j1+joff] += t1;
               }
            }
         }
      }
      ns = ns2;
   }
/* unscramble coefficients and normalize */
   kmr = nxy/nx;
   for (j = 1; j < nxhh; j++) {
      t1 = cimagf(sct[kmr*j]) - crealf(sct[kmr*j])*_Complex_I;
      for (k = kypi-1; k < kypt; k++) {
         joff = ndim*nxvh*k;
         for (jj = 0; jj < ndim; jj++) {
            t = conjf(f[jj+ndim*(nxh-j)+joff]);
            s = f[jj+ndim*j+joff] + t;
            t = (f[jj+ndim*j+joff] - t)*t1;
            f[jj+ndim*j+joff] = ani*(s + t);
            f[jj+ndim*(nxh-j)+joff] = ani*conjf(s - t);
         }
      }
   }
   ani = 2.0*ani;
   for (k = kypi-1; k < kypt; k++) {
      joff = ndim*nxvh*k;
      for (jj = 0; jj < ndim; jj++) {
         f[jj+joff] = ani*((crealf(f[jj+joff]) + cimagf(f[jj+joff]))
                         + (crealf(f[jj+joff]) - cimagf(f[jj+joff]))*_Complex_I);
         if (nxhh > 0)
            f[jj+ndim*nxhh+joff] = ani*conjf(f[jj+ndim*nxhh+joff]);
      }
   }
   return;
/* forward fourier transform */
L140: kmr = nxy/nx;
/* scramble coefficients */
   for (j = 1; j < nxhh; j++) {
      t1 = cimagf(sct[kmr*j]) + crealf(sct[kmr*j])*_Complex_I;
      for (k = kypi-1; k < kypt; k++) {
         joff = ndim*nxvh*k;
         for (jj = 0; jj < ndim; jj++) {
            t = conjf(f[jj+ndim*(nxh-j)+joff]);
            s = f[jj+ndim*j+joff] + t;
            t = (f[jj+ndim*j+joff] - t)*t1;
            f[jj+ndim*j+joff] = s + t;
            f[jj+ndim*(nxh-j)+joff] = conjf(s - t);
         }
      }
   }
   for (k = kypi-1; k < kypt; k++) {
      joff = ndim*nxvh*k;
      for (jj = 0; jj < ndim; jj++) {
         f[jj+joff] = (crealf(f[jj+joff]) + cimagf(f[jj+joff]))
                    + (crealf(f[jj+joff]) - cimagf(f[jj+joff]))*_Complex_I;
         if (nxhh > 0)
            f[jj+ndim*nxhh+joff] = 2.0*conjf(f[jj+ndim*nxhh+joff]);
      }
   }
   nrx = nxhy/nxh;
/* bit-reverse array elements in x */
   for (j = 0; j < nxh; j++) {
      j1 = (mixup[j] - 1)/nrx;
      if (j >= j1)
         continue;
      for (k = kypi-1; k < kypt; k++) {
         joff = ndim*nxvh*k;
         for (jj = 0; jj < ndim; jj++) {
            t1 = f[jj+ndim*j1+joff];
            f[jj+ndim*j1+joff] = f[jj+ndim*j+joff];
            f[jj+ndim*j+joff] = t1;
         }
      }
   }
/* then transform in x */
   nrx = nxy/nxh;
   ns = 1;
   for (m = 0; m < indx1; m++) {
      ns2 = ns + ns;
      km = nxhh/ns;
      kmr = km*nrx;
      for (k = 0; k < km; k++) {
         k1 = ns2*k;
         k2 = k1 + ns;
         for (j = 0; j < ns; j++) {
            j1 = j + k1;
            j2 = j + k2;
            s = conjf(sct[kmr*j]);
            for (i = kypi-1; i < kypt; i++) {
               joff = ndim*nxvh*i;
               for (jj = 0; jj < ndim; jj++) {
                  t1 = s*f[jj+ndim*j2+joff];
                  f[jj+ndim*j2+joff] = f[jj+ndim*j1+joff] - t1;
                  f[jj+ndim*j1+joff] +=  t1;
               }
            }
         }
      }
      ns = ns2;
   }
/* swap complex components */
   cppswapc2n((float *)f,(float *)ss,isign,nxh,kypi,kypt,nxvh,kypd,
              ndim);
   return;
}

/*--------------------------------------------------------------------*/
void cppfft2rnxy(float complex g[], int isign, int mixup[],
                 float complex sct[], int indx, int indy, int kstrt,
                 int kxpi, int kxpp, int nyv, int kxp, int ndim,
                 int nxhyd, int nxyhd) {
/* this subroutine performs the y part of N two dimensional real to
   complex fast fourier transforms and their inverses, for a subset of x,
   using complex arithmetic, where N = ndim
   for data which is distributed in blocks
   using complex arithmetic, for data which is distributed in blocks
   for isign = (-1,1), input: all, output: g
   for isign = -1, approximate flop count: M*(5*log2(N) + 10)/nvp
   for isign = 1,  approximate flop count: M*(5*log2(N) + 8)/nvp
   where M = (nx/2)*ny, and nvp = number of procs
   indx/indy = exponent which determines length in x/y direction,
   where nx=2**indx, ny=2**indy
   if isign = -1, an inverse fourier transform is performed
   g[n][m][0:N-1] = sum(g[j][k][0:N-1]*exp(-sqrt(-1)*2pi*m*k/ny))
   if isign = 1, a forward fourier transform is performed
   g[j][k][0:N-1] = sum(g[n][m][0:N-1]*exp(sqrt(-1)*2pi*m*k/ny))
   kstrt = starting data block number
   kxpi = initial x index used
   kxpp = number of x indices used
   nyv = first dimension of g
   kxp = number of data values per block in x
   ndim = leading dimension of arrays f and g
   mixup = array of bit reversed addresses
   sct = sine/cosine table
   nxhyd = maximum of (nx/2,ny)
   nxyhd = one half of maximum of (nx,ny)
   the real data is stored in a complex array of length nx/2, ny
   with the odd/even x points stored in the real/imaginary parts.
   in complex notation, fourier coefficients are stored as follows:
   g[j][k][0:N-1] = mode jj,k, where jj = j + kxp*(kstrt - 1)
   0 <= jj < nx/2 and 0 <= k < ny, except for
   g[0][k][0:N-1] = mode nx/2,k, where ny/2+1 <= k < ny, and
   imaginary part of g[0][0][0:N-1] = real part of mode nx/2,0 and
   imaginary part of g[0][ny/2][0:N-1] = real part of mode nx/2,ny/2
   on node kstrt=0
   written by viktor k. decyk, ucla
   parallel, RISC optimized version
local data                                                            */
   int indx1, indx1y, nx, nxh, ny, nyh;
   int nxy, nxhy, ks, kxpt, j, k, nry;
   int i, m, ns, ns2, km, kmr, k1, k2, j1, j2, jj, koff;
   float complex s, t1;
   indx1 = indx - 1;
   indx1y = indx1 > indy ? indx1 : indy;
   nx = 1L<<indx;
   nxh = nx/2;
   ny = 1L<<indy;
   nyh = ny/2;
   nxy = nx > ny ? nx : ny;
   nxhy = 1L<<indx1y;
   ks = kstrt - 1;
   kxpt = kxpi + kxpp - 1;
   if (kstrt > nxh)
      return;
   if (isign > 0)
      goto L90;
/* inverse fourier transform */
   nry = nxhy/ny;
/* bit-reverse array elements in y */
   for (k = 0; k < ny; k++) {
      k1 = (mixup[k] - 1)/nry;
      if (k >= k1)
         continue;
      for (j = kxpi-1; j < kxpt; j++) {
         koff = ndim*nyv*j;
         for (jj = 0; jj < ndim; jj++) {
            t1 = g[jj+ndim*k1+koff];
            g[jj+ndim*k1+koff] = g[jj+ndim*k+koff];
            g[jj+ndim*k+koff] = t1;
         }
      }
   }
/* then transform in y */
   nry = nxy/ny;
   ns = 1;
   for (m = 0; m < indy; m++) {
      ns2 = ns + ns;
      km = nyh/ns;
      kmr = km*nry;
      for (k = 0; k < km; k++) {
         k1 = ns2*k;
         k2 = k1 + ns;
         for (j = 0; j < ns; j++) {
            j1 = j + k1;
            j2 = j + k2;
            s = sct[kmr*j];
            for (i = kxpi-1; i < kxpt; i++) {
               koff = ndim*nyv*i;
               for (jj = 0; jj < ndim; jj++) {
                  t1 = s*g[jj+ndim*j2+koff];
                  g[jj+ndim*j2+koff] = g[jj+ndim*j1+koff] - t1;
                  g[jj+ndim*j1+koff] += t1;
               }
            }
         }
      }
      ns = ns2;
   }
/* unscramble modes kx = 0, nx/2 */
   if (ks > 0)
      return;
   for (k = 1; k < nyh; k++) {
      if (kxpi==1) {
         for (jj = 0; jj < ndim; jj++) {
            s = g[jj+ndim*(ny-k)];
            g[jj+ndim*(ny-k)] = 0.5*(cimagf(g[jj+ndim*k] + s)
                                   + crealf(g[jj+ndim*k] - s)*_Complex_I);
            g[jj+ndim*k] = 0.5*(crealf(g[jj+ndim*k] + s)
                              + cimagf(g[jj+ndim*k] - s)*_Complex_I);
         }
      }
   }
   return;
/* forward fourier transform */
/* scramble modes kx = 0, nx/2 */
L90: if (ks==0) {
      for (k = 1; k < nyh; k++) {
         if (kxpi==1) {
            for (jj = 0; jj < ndim; jj++) {
               s = cimagf(g[jj+ndim*(ny-k)])
                 + crealf(g[jj+ndim*(ny-k)])*_Complex_I;
               g[jj+ndim*(ny-k)] = conjf(g[jj+ndim*k] - s);
               g[jj+ndim*k] += s;
            }
         }
      }
   }
   nry = nxhy/ny;
/* bit-reverse array elements in y */
   for (k = 0; k < ny; k++) {
      k1 = (mixup[k] - 1)/nry;
      if (k >= k1)
         continue;
      for (j = kxpi-1; j < kxpt; j++) {
         koff = ndim*nyv*j;
         for (jj = 0; jj < ndim; jj++) {
            t1 = g[jj+ndim*k1+koff];
            g[jj+ndim*k1+koff] = g[jj+ndim*k+koff];
            g[jj+ndim*k+koff] = t1;
         }
      }
   }
/* first transform in y */
   nry = nxy/ny;
   ns = 1;
   for (m = 0; m < indy; m++) {
      ns2 = ns + ns;
      km = nyh/ns;
      kmr = km*nry;
      for (k = 0; k < km; k++) {
         k1 = ns2*k;
         k2 = k1 + ns;
         for (j = 0; j < ns; j++) {
            j1 = j + k1;
            j2 = j + k2;
            s = conjf(sct[kmr*j]);
            for (i = kxpi-1; i < kxpt; i++) {
               koff = ndim*nyv*i;
               for (jj = 0; jj < ndim; jj++) {
                  t1 = s*g[jj+ndim*j2+koff];
                  g[jj+ndim*j2+koff] = g[jj+ndim*j1+koff] - t1;
                  g[jj+ndim*j1+koff] += t1;
               }
            }
         }
      }
      ns = ns2;
   }
   return;
}

/*--------------------------------------------------------------------*/
void cwppfft2r(float complex f[], float complex g[], float complex bs[],
               float complex br[], int isign, int ntpose, int mixup[],
               float complex sct[], float *ttp, int indx, int indy,
               int kstrt, int nvp, int nxvh, int nyv, int kxp, int kyp,
               int kypd, int nxhyd, int nxyhd) {
/* wrapper function for parallel real to complex fft */
/* local data */
   int nxh, ny, ks, kxpp, kypp;
   static int kxpi = 1, kypi = 1;
   float tf;
   double dtime;
/* calculate range of indices */
   nxh = 1L<<(indx - 1);
   ny = 1L<<indy;
   ks = kstrt - 1;
   kxpp = nxh - kxp*ks;
   kxpp = 0 > kxpp ? 0 : kxpp;
   kxpp = kxp < kxpp ? kxp : kxpp;
   kypp = ny - kyp*ks;
   kypp = 0 > kypp ? 0 : kypp;
   kypp = kyp < kypp ? kyp : kypp;
/* inverse fourier transform */
   if (isign < 0) {
/* perform x fft */
      cppfft2rxx(f,isign,mixup,sct,indx,indy,kstrt,kypi,kypp,nxvh,kypd,
                 nxhyd,nxyhd);
/* transpose f array to g */
      cpwtimera(-1,ttp,&dtime);
      cpptpose(f,g,bs,br,nxh,ny,kxp,kyp,kstrt,nvp,nxvh,nyv,kxp,kypd);
      cpwtimera(1,ttp,&dtime);
/* perform y fft */
      cppfft2rxy(g,isign,mixup,sct,indx,indy,kstrt,kxpi,kxpp,nyv,kxp,
                 nxhyd,nxyhd);
/* transpose g array to f */
      if (ntpose==0) {
         cpwtimera(-1,&tf,&dtime);
         cpptpose(g,f,br,bs,ny,nxh,kyp,kxp,kstrt,nvp,nyv,nxvh,kypd,kxp);
         cpwtimera(1,&tf,&dtime);
      }
   }
/* forward fourier transform */
   else if (isign > 0) {
/* transpose f array to g */
      if (ntpose==0) {
         cpwtimera(-1,&tf,&dtime);
         cpptpose(f,g,bs,br,nxh,ny,kxp,kyp,kstrt,nvp,nxvh,nyv,kxp,kypd);
         cpwtimera(1,&tf,&dtime);
      }
/* perform y fft */
      cppfft2rxy(g,isign,mixup,sct,indx,indy,kstrt,kxpi,kxpp,nyv,kxp,
                 nxhyd,nxyhd);
/* transpose g array to f */
      cpwtimera(-1,ttp,&dtime);
      cpptpose(g,f,br,bs,ny,nxh,kyp,kxp,kstrt,nvp,nyv,nxvh,kypd,kxp);
      cpwtimera(1,ttp,&dtime);
/* perform x fft */
      cppfft2rxx(f,isign,mixup,sct,indx,indy,kstrt,kypi,kypp,nxvh,kypd,
                 nxhyd,nxyhd);
   }
   if (ntpose==0)
      *ttp += tf;
   return;
}

/*--------------------------------------------------------------------*/
void cwppfft2r3(float complex f[], float complex g[], float complex bs[],
                float complex br[], int isign, int ntpose, int mixup[],
                float complex sct[], float *ttp, int indx, int indy,
                int kstrt, int nvp, int nxvh, int nyv, int kxp, int kyp,
                int kypd, int nxhyd, int nxyhd) {
/* wrapper function for parallel real to complex fft */
/* local data */
   int nxh, ny, ks, kxpp, kypp;
   static int kxpi = 1, kypi = 1;
   float tf;
   double dtime;
/* calculate range of indices */
   nxh = 1L<<(indx - 1);
   ny = 1L<<indy;
   ks = kstrt - 1;
   kxpp = nxh - kxp*ks;
   kxpp = 0 > kxpp ? 0 : kxpp;
   kxpp = kxp < kxpp ? kxp : kxpp;
   kypp = ny - kyp*ks;
   kypp = 0 > kypp ? 0 : kypp;
   kypp = kyp < kypp ? kyp : kypp;
/* inverse fourier transform */
   if (isign < 0) {
/* perform x fft */
      cppfft2r3xx(f,isign,mixup,sct,indx,indy,kstrt,kypi,kypp,nxvh,kypd,
                  nxhyd,nxyhd);
/* transpose f array to g */
      cpwtimera(-1,ttp,&dtime);
      cppntpose(f,g,bs,br,nxh,ny,kxp,kyp,kstrt,nvp,3,nxvh,nyv,kxp,kypd);
      cpwtimera(1,ttp,&dtime);
/* perform y fft */
      cppfft2r3xy(g,isign,mixup,sct,indx,indy,kstrt,kxpi,kxpp,nyv,kxp,
                  nxhyd,nxyhd);
/* transpose g array to f */
      if (ntpose==0) {
         cpwtimera(-1,&tf,&dtime);
         cppntpose(g,f,br,bs,ny,nxh,kyp,kxp,kstrt,nvp,3,nyv,nxvh,kypd,
                   kxp);
         cpwtimera(1,&tf,&dtime);
      }
   }
/* forward fourier transform */
   else if (isign > 0) {
/* transpose f array to g */
      if (ntpose==0) {
         cpwtimera(-1,&tf,&dtime);
         cppntpose(f,g,bs,br,nxh,ny,kxp,kyp,kstrt,nvp,3,nxvh,nyv,kxp,
                   kypd);
         cpwtimera(1,&tf,&dtime);
      }
/* perform y fft */
      cppfft2r3xy(g,isign,mixup,sct,indx,indy,kstrt,kxpi,kxpp,nyv,kxp,
                  nxhyd,nxyhd);
/* transpose g array to f */
      cpwtimera(-1,ttp,&dtime);
      cppntpose(g,f,br,bs,ny,nxh,kyp,kxp,kstrt,nvp,3,nyv,nxvh,kypd,kxp);
      cpwtimera(1,ttp,&dtime);
/* perform x fft */
      cppfft2r3xx(f,isign,mixup,sct,indx,indy,kstrt,kypi,kypp,nxvh,kypd,
                  nxhyd,nxyhd);
   }
   if (ntpose==0)
      *ttp += tf;
   return;
}

/*--------------------------------------------------------------------*/
void cwppfft2rn(float complex f[], float complex g[],
                float complex bs[], float complex br[],
                float complex ss[], int isign, int ntpose, int mixup[],
                float complex sct[], float *ttp, int indx, int indy,
                int kstrt, int nvp, int nxvh, int nyv, int kxp, int kyp,
                int kypd, int ndim, int nxhyd, int nxyhd) {
/* wrapper function for parallel real to complex fft */
/* local data */
   int nxh, ny, ks, kxpp, kypp;
   static int kxpi = 1, kypi = 1;
   float tf;
   double dtime;
/* calculate range of indices */
   nxh = 1L<<(indx - 1);
   ny = 1L<<indy;
   ks = kstrt - 1;
   kxpp = nxh - kxp*ks;
   kxpp = 0 > kxpp ? 0 : kxpp;
   kxpp = kxp < kxpp ? kxp : kxpp;
   kypp = ny - kyp*ks;
   kypp = 0 > kypp ? 0 : kypp;
   kypp = kyp < kypp ? kyp : kypp;
/* inverse fourier transform */
   if (isign < 0) {
/* perform x fft */
      cppfft2rnxx(f,ss,isign,mixup,sct,indx,indy,kstrt,kypi,kypp,nxvh,
                  kypd,ndim,nxhyd,nxyhd);
/* transpose f array to g */
      cpwtimera(-1,ttp,&dtime);
      cppntpose(f,g,bs,br,nxh,ny,kxp,kyp,kstrt,nvp,ndim,nxvh,nyv,kxp,
                kypd);
      cpwtimera(1,ttp,&dtime);
/* perform y fft */
      cppfft2rnxy(g,isign,mixup,sct,indx,indy,kstrt,kxpi,kxpp,nyv,kxp,
                  ndim,nxhyd,nxyhd);
/* transpose g array to f */
      if (ntpose==0) {
         cpwtimera(-1,&tf,&dtime);
         cppntpose(g,f,br,bs,ny,nxh,kyp,kxp,kstrt,nvp,ndim,nyv,nxvh,
                   kypd,kxp);
         cpwtimera(1,&tf,&dtime);
      }
   }
/* forward fourier transform */
   else if (isign > 0) {
/* transpose f array to g */
      if (ntpose==0) {
         cpwtimera(-1,&tf,&dtime);
         cppntpose(f,g,bs,br,nxh,ny,kxp,kyp,kstrt,nvp,ndim,nxvh,nyv,
                   kxp,kypd);
         cpwtimera(1,&tf,&dtime);
      }
/* perform y fft */
      cppfft2rnxy(g,isign,mixup,sct,indx,indy,kstrt,kxpi,kxpp,nyv,kxp,
                  ndim,nxhyd,nxyhd);
/* transpose g array to f */
      cpwtimera(-1,ttp,&dtime);
      cppntpose(g,f,br,bs,ny,nxh,kyp,kxp,kstrt,nvp,ndim,nyv,nxvh,kypd,
                kxp);
      cpwtimera(1,ttp,&dtime);
/* perform x fft */
      cppfft2rnxx(f,ss,isign,mixup,sct,indx,indy,kstrt,kypi,kypp,nxvh,
                  kypd,ndim,nxhyd,nxyhd);
   }
   if (ntpose==0)
      *ttp += tf;
   return;
}

/*--------------------------------------------------------------------*/
void cppswapc2n(float f[], float s[], int isign, int nxh, int kypi,
                int kypt, int nxvh, int kypd, int ndim) {
/* this subroutine swaps components for multiple ffts
   f = input  array
   s = scratch array
   isign = (-1,1) = swap (real-to-complex,complex-to-real)
   nxh = complex dimension in x direction
   kypi/kypt = initial/final y index used
   nxvh = half of the second dimension of f
   kypd = third dimension of f
   ndim = leading dimension of array f
local data                                                            */
   int i, j, k, ioff, nk;
/* swap complex components */
/* real to complex */
   if (isign < 0){
      for (k = kypi-1; k < kypt; k++) {
         nk = 2*ndim*nxvh*k;
         for (j = 0; j < nxh; j++) {
            ioff = 2*ndim*j;
            for (i = 0; i < ndim; i++) {
               s[2*i+ioff] = f[i+ndim*(2*j)+nk];
               s[2*i+ioff+1] = f[i+ndim*(2*j+1)+nk];
            }
         }
         for (j = 0; j < nxh; j++) {
            ioff = 2*ndim*j;
            for (i = 0; i < ndim; i++) {
               f[i+ndim*(2*j)+nk] = s[i+ioff];
            }
            ioff += ndim;
            for (i = 0; i < ndim; i++) {
               f[i+ndim*(2*j+1)+nk] = s[i+ioff];
            }
         }
      }
   }
/* complex to real */
   else if (isign > 0) {
      for (k = kypi-1; k < kypt; k++) {
         nk = 2*ndim*nxvh*k;
         for (j = 0; j < nxh; j++) {
            ioff = 2*ndim*j;
            for (i = 0; i < ndim; i++) {
               s[i+ioff] = f[i+ndim*(2*j)+nk];
            }
            ioff += ndim;
            for (i = 0; i < ndim; i++) {
               s[i+ioff] = f[i+ndim*(2*j+1)+nk];
            }
         }
         for (j = 0; j < nxh; j++) {
            ioff = 2*ndim*j;
            for (i = 0; i < ndim; i++) {
               f[i+ndim*(2*j)+nk] = s[2*i+ioff];
               f[i+ndim*(2*j+1)+nk] = s[2*i+ioff+1];
            }
         }
      }
   }
   return;
}

/* Interfaces to Fortran */

/*--------------------------------------------------------------------*/
void cpdicomp2l_(float *edges, int *nyp, int *noff, int *nypmx,
                 int *nypmn, int *ny, int *kstrt, int *nvp, int *idps) {
   cpdicomp2l(edges,nyp,noff,nypmx,nypmn,*ny,*kstrt,*nvp,*idps);
   return;
}

/*--------------------------------------------------------------------*/
void cpdistr2h_(float *part, float *edges, int *npp, int *nps,
                float *vtx, float *vty, float *vtz, float *vdx,
                float *vdy, float *vdz, int *npx, int *npy, int *nx,
                int *ny, int *idimp, int *npmax, int *idps, int *ipbc,
                int *ierr) {
   cpdistr2h(part,edges,npp,*nps,*vtx,*vty,*vtz,*vdx,*vdy,*vdz,*npx,
             *npy,*nx,*ny,*idimp,*npmax,*idps,*ipbc,ierr);
   return;
}

/*--------------------------------------------------------------------*/
void cppgbpush23l_(float *part, float *fxy, float *bxy, float *edges,
                   int *npp, int *noff, int *ihole, float *qbm,
                   float *dt, float *dtc, float *ek, int *nx, int *ny,
                   int *idimp, int *npmax, int *nxv, int *nypmx,
                   int *idps, int *ntmax, int *ipbc) {
   cppgbpush23l(part,fxy,bxy,edges,*npp,*noff,ihole,*qbm,*dt,*dtc,ek,
                *nx,*ny,*idimp,*npmax,*nxv,*nypmx,*idps,*ntmax,*ipbc);
   return;
}

/*--------------------------------------------------------------------*/
void cppgpost2l_(float *part, float *q, int *npp, int *noff, float *qm,
                 int *idimp, int *npmax, int *nxv, int *nypmx) {
   cppgpost2l(part,q,*npp,*noff,*qm,*idimp,*npmax,*nxv,*nypmx);
   return;
}

/*--------------------------------------------------------------------*/
void cppgjpost2l_(float *part, float *cu, float *edges, int *npp,
                  int *noff, int *ihole, float *qm, float *dt, int *nx,
                  int *ny, int *idimp, int *npmax, int *nxv, int *nypmx,
                  int *idps, int *ntmax, int *ipbc) {
   cppgjpost2l(part,cu,edges,*npp,*noff,ihole,*qm,*dt,*nx,*ny,*idimp,
               *npmax,*nxv,*nypmx,*idps,*ntmax,*ipbc);
   return;
}

/*--------------------------------------------------------------------*/
void cppgmjpost2l_(float *part, float *amu, int *npp, int *noff,
                   float *qm, int *idimp, int *npmax, int *nxv,
                   int *nypmx) {
   cppgmjpost2l(part,amu,*npp,*noff,*qm,*idimp,*npmax,*nxv,*nypmx);
   return;
}

/*--------------------------------------------------------------------*/
void cppgdjpost2l_(float *part, float *fxy, float *bxy, int *npp,
                   int *noff, float *dcu, float *amu, float *qm,
                   float *qbm, float *dt, int *idimp, int *npmax,
                    int *nxv, int *nypmx) {
   cppgdjpost2l(part,fxy,bxy,*npp,*noff,dcu,amu,*qm,*qbm,*dt,*idimp,
                *npmax,*nxv,*nypmx);
   return;
}

/*--------------------------------------------------------------------*/
void cppgdcjpost2l_(float *part, float *fxy, float *bxy, int *npp,
                    int *noff, float *cu, float *dcu, float *amu,
                    float *qm, float *qbm, float *dt, int *idimp,
                    int *npmax, int *nxv, int *nypmx) {
   cppgdcjpost2l(part,fxy,bxy,*npp,*noff,cu,dcu,amu,*qm,*qbm,*dt,*idimp,
                 *npmax,*nxv,*nypmx);
   return;
}

/*--------------------------------------------------------------------*/
void cppdsortp2yl_(float *parta, float *partb, int *npic, int *npp,
                   int *noff, int *nyp, int *idimp, int *npmax,
                   int *nypm1) {
   cppdsortp2yl(parta,partb,npic,*npp,*noff,*nyp,*idimp,*npmax,*nypm1);
   return;
}

/*--------------------------------------------------------------------*/
void cppcguard2xl_(float *fxy, int *myp, int *nx, int *ndim, int *nxe,
                   int *nypmx) {
   cppcguard2xl(fxy,*myp,*nx,*ndim,*nxe,*nypmx);
   return;
}

/*--------------------------------------------------------------------*/
void cppaguard2xl_(float *q, int *myp, int *nx, int *nxe, int *nypmx) {
   cppaguard2xl(q,*myp,*nx,*nxe,*nypmx);
   return;
}

/*--------------------------------------------------------------------*/
void cppacguard2xl_(float *cu, int *myp, int *nx, int *ndim, int *nxe,
                    int *nypmx) {
   cppacguard2xl(cu,*myp,*nx,*ndim,*nxe,*nypmx);
   return;
}

/*--------------------------------------------------------------------*/
void cppascfguard2l_(float *dcu, float *cus, int *nyp, float *q2m0,
                     int *nx, int *nxe, int *nypmx) {
   cppascfguard2l(dcu,cus,*nyp,*q2m0,*nx,*nxe,*nypmx);
   return;
}

/*--------------------------------------------------------------------*/
void cppfwpminmx2_(float *qe, int *nyp, float *qbme, float *wpmax,
                   float *wpmin, int *nx, int *nxe, int *nypmx) {
   cppfwpminmx2(qe,*nyp,*qbme,wpmax,wpmin,*nx,*nxe,*nypmx);
   return;
}

/*--------------------------------------------------------------------*/
void cppois23_(float complex *q, float complex *fxy, int *isign,
               float complex *ffc, float *ax, float *ay, float *affp,
               float *we, int *nx, int *ny, int *kstrt, int *nyv,
               int *kxp, int *nyhd) {
   cppois23(q,fxy,*isign,ffc,*ax,*ay,*affp,we,*nx,*ny,*kstrt,*nyv,*kxp,
            *nyhd);
   return;
}

/*--------------------------------------------------------------------*/
void cppcuperp2_(float complex *cu, int *nx, int *ny, int *kstrt,
                 int *nyv, int *kxp) {
   cppcuperp2(cu,*nx,*ny,*kstrt,*nyv,*kxp);
   return;
}

/*--------------------------------------------------------------------*/
void cppbbpoisp23_(float complex *cu, float complex *bxy,
                   float complex *ffc, float *ci, float *wm, int *nx,
                   int *ny, int *kstrt, int *nyv, int *kxp, int *nyhd) {
   cppbbpoisp23(cu,bxy,ffc,*ci,wm,*nx,*ny,*kstrt,*nyv,*kxp,*nyhd);
   return;
}

/*--------------------------------------------------------------------*/
void cppbaddext2_(float *bxy, int *nyp, float *omx, float *omy,
                  float *omz, int *nx, int *nxe, int *nypmx) {
   cppbaddext2(bxy,*nyp,*omx,*omy,*omz,*nx,*nxe,*nypmx);
   return;
}

/*--------------------------------------------------------------------*/
void cppdcuperp23_(float complex *dcu, float complex *amu, int *nx,
                   int *ny, int *kstrt, int *nyv, int *kxp) {
   cppdcuperp23(dcu,amu,*nx,*ny,*kstrt,*nyv,*kxp);
   return;
}

/*--------------------------------------------------------------------*/
void cppadcuperp23_(float complex *dcu, float complex *amu, int *nx,
                    int *ny, int *kstrt, int *nyv, int *kxp) {
   cppadcuperp23(dcu,amu,*nx,*ny,*kstrt,*nyv,*kxp);
   return;
}

/*--------------------------------------------------------------------*/
void cppepoisp23_(float complex *dcu, float complex *exy, int *isign,
                  float complex *ffe, float *ax, float *ay, float *affp,
                  float *wp0, float *ci, float *wf, int *nx, int *ny,
                  int *kstrt, int *nyv, int *kxp, int *nyhd) {
   cppepoisp23(dcu,exy,*isign,ffe,*ax,*ay,*affp,*wp0,*ci,wf,*nx,*ny,
               *kstrt,*nyv,*kxp,*nyhd);
   return;
}

/*--------------------------------------------------------------------*/
void cppaddvrfield2_(float *a, float *b, float *c, int *ndim, int *nxe,
                     int *nypmx) {
   cppaddvrfield2(a,b,c,*ndim,*nxe,*nypmx);
   return;
}

/*--------------------------------------------------------------------*/
void cwpfft2rinit_(int *mixup, float complex *sct, int *indx, int *indy,
                   int *nxhyd, int *nxyhd) {
   cwpfft2rinit(mixup,sct,*indx,*indy,*nxhyd,*nxyhd);
   return;
}

/*--------------------------------------------------------------------*/
void cppfft2rxx_(float complex *f, int *isign, int *mixup,
                 float complex *sct, int *indx, int *indy, int *kstrt,
                 int *kypi, int *kypp, int *nxvh, int *kypd, int *nxhyd,
                 int *nxyhd) {
   cppfft2rxx(f,*isign,mixup,sct,*indx,*indy,*kstrt,*kypi,*kypp,*nxvh,
              *kypd,*nxhyd,*nxyhd);
   return;
}

/*--------------------------------------------------------------------*/
void cppfft2rxy_(float complex *g, int *isign, int *mixup,
                 float complex *sct, int *indx, int *indy, int *kstrt,
                 int *kxpi, int *kxpp, int *nyv, int *kxp, int *nxhyd,
                 int *nxyhd) {
   cppfft2rxy(g,*isign,mixup,sct,*indx,*indy,*kstrt,*kxpi,*kxpp,*nyv,
              *kxp,*nxhyd,*nxyhd);
   return;
}

/*--------------------------------------------------------------------*/
void cppfft2r3xx_(float complex *f, int *isign, int *mixup,
                  float complex *sct, int *indx, int *indy, int *kstrt,
                  int *kypi, int *kypp, int *nxvh, int *kypd, int *nxhyd,
                  int *nxyhd) {
   cppfft2r3xx(f,*isign,mixup,sct,*indx,*indy,*kstrt,*kypi,*kypp,*nxvh,
               *kypd,*nxhyd,*nxyhd);
   return;
}

/*--------------------------------------------------------------------*/
void cppfft2r3xy_(float complex *g, int *isign, int *mixup,
                  float complex *sct, int *indx, int *indy, int *kstrt,
                  int *kxpi, int *kxpp, int *nyv, int *kxp, int *nxhyd,
                  int *nxyhd) {
   cppfft2r3xy(g,*isign,mixup,sct,*indx,*indy,*kstrt,*kxpi,*kxpp,*nyv,
               *kxp,*nxhyd,*nxyhd);
   return;
}

/*--------------------------------------------------------------------*/
void cppfft2rnxx_(float complex *f, float complex *ss, int *isign,
                  int *mixup, float complex *sct, int *indx, int *indy,
                  int *kstrt, int *kypi, int *kypp, int *nxvh,
                  int *kypd, int *ndim, int *nxhyd, int *nxyhd) {
   cppfft2rnxx(f,ss,*isign,mixup,sct,*indx,*indy,*kstrt,*kypi,*kypp,
               *nxvh,*kypd,*ndim,*nxhyd,*nxyhd);
   return;
}

/*--------------------------------------------------------------------*/
void cppfft2rnxy_(float complex *g, int *isign, int *mixup,
                  float complex *sct, int *indx, int *indy, int *kstrt,
                  int *kxpi, int *kxpp, int *nyv, int *kxp, int *ndim,
                  int *nxhyd, int *nxyhd) {
   cppfft2rnxy(g,*isign,mixup,sct,*indx,*indy,*kstrt,*kxpi,*kxpp,*nyv,
               *kxp,*ndim,*nxhyd,*nxyhd);
   return;
}

/*--------------------------------------------------------------------*/
void cwppfft2r_(float complex *f, float complex *g, float complex *bs,
                float complex *br, int *isign, int *ntpose, int *mixup,
                float complex *sct, float *ttp, int *indx, int *indy,
                int *kstrt, int *nvp, int *nxvh, int *nyv, int *kxp,
                int *kyp, int *kypd, int *nxhyd, int *nxyhd) {
   cwppfft2r(f,g,bs,br,*isign,*ntpose,mixup,sct,ttp,*indx,*indy,*kstrt,
             *nvp,*nxvh,*nyv,*kxp,*kyp,*kypd,*nxhyd,*nxyhd);
   return;
}

/*--------------------------------------------------------------------*/
void cwppfft2r3_(float complex *f, float complex *g, float complex *bs,
                 float complex *br, int *isign, int *ntpose, int *mixup,
                 float complex *sct, float *ttp, int *indx, int *indy,
                 int *kstrt, int *nvp, int *nxvh, int *nyv, int *kxp,
                 int *kyp, int *kypd, int *nxhyd, int *nxyhd) {
   cwppfft2r3(f,g,bs,br,*isign,*ntpose,mixup,sct,ttp,*indx,*indy,*kstrt,
              *nvp,*nxvh,*nyv,*kxp,*kyp,*kypd,*nxhyd,*nxyhd);
   return;
}

/*--------------------------------------------------------------------*/
void cwppfft2rn_(float complex *f, float complex *g,
                 float complex *bs, float complex *br,
                 float complex *ss, int *isign, int *ntpose, int *mixup,
                 float complex *sct, float *ttp, int *indx, int *indy,
                 int *kstrt, int *nvp, int *nxvh, int *nyv, int *kxp,
                 int *kyp, int *kypd, int *ndim, int *nxhyd,
                 int *nxyhd) {
   cwppfft2rn(f,g,bs,br,ss,*isign,*ntpose,mixup,sct,ttp,*indx,*indy,
              *kstrt,*nvp,*nxvh,*nyv,*kxp,*kyp,*kypd,*ndim,*nxhyd,
              *nxyhd);
   return;
}

/*--------------------------------------------------------------------*/
void cppswapc2n_(float *f, float *s, int *isign, int *nxh, int *kypi,
                 int *kypt, int *nxvh, int *kypd, int *ndim) {
   cppswapc2n(f,s,*isign,*nxh,*kypi,*kypt,*nxvh,*kypd,*ndim);
   return;
}
