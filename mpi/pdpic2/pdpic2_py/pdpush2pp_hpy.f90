!-----------------------------------------------------------------------
! Interface file for pdpush2.f
!     module pdpush2_h
!     implicit none
!
      interface
         subroutine PDICOMP2L(edges,nyp,noff,nypmx,nypmn,ny,kstrt,nvp,  &
     &idps)
         implicit none
         integer, intent(in) :: ny, kstrt, nvp, idps
         integer, intent(inout) :: nyp, noff, nypmx, nypmn
!        real, dimension(idps), intent(inout) :: edges
         real, dimension(*), intent(inout) :: edges
         end subroutine
      end interface
!
      interface
         subroutine PDISTR2H(part,edges,npp,nps,vtx,vty,vtz,vdx,vdy,vdz,&
     &npx,npy,nx,ny,idimp,npmax,idps,ipbc,ierr)
         implicit none
         integer, intent(in) :: nps, npx, npy, nx, ny, idimp, npmax
         integer, intent(in) :: idps, ipbc
         integer, intent(inout) :: npp, ierr
         real, intent(in) :: vtx, vty, vtz, vdx, vdy, vdz
!        real, dimension(idimp,npmax), intent(inout) :: part
         real, dimension(*), intent(inout) :: part
!        real, dimension(idps), intent(in) :: edges
         real, dimension(*), intent(in) :: edges
         end subroutine
      end interface
!
      interface
         subroutine PPGBPUSH23L(part,fxy,bxy,edges,npp,noff,ihole,qbm,dt&
     &,dtc,ek,nx,ny,idimp,npmax,nxv,nypmx,idps,ntmax,ipbc)
         implicit none
         integer, intent(in) :: npp, noff, nx, ny, idimp, npmax, idps
         integer, intent(in) :: ntmax, nxv, nypmx, ipbc
         real, intent(in) :: qbm, dt, dtc
         real, intent(inout) :: ek
!        real, dimension(idimp,npmax), intent(inout) :: part
         real, dimension(*), intent(inout) :: part
!        real, dimension(3,nxv,nypmx), intent(in) :: fxy, bxy
         real, dimension(*), intent(in) :: fxy, bxy
!        real, dimension(idps), intent(in) :: edges
         real, dimension(*), intent(in) :: edges
!        integer, dimension(ntmax+1), intent(inout) :: ihole
         integer, dimension(*), intent(inout) :: ihole
         end subroutine
      end interface
!
      interface
         subroutine PPGPOST2L(part,q,npp,noff,qm,idimp,npmax,nxv,nypmx)
         implicit none
         integer, intent(in) :: npp, noff, idimp, npmax, nxv, nypmx
         real, intent(in) :: qm
!        real, dimension(idimp,npmax), intent(in) :: part
         real, dimension(*), intent(in) :: part
!        real, dimension(nxv,nypmx), intent(inout) :: q
         real, dimension(*), intent(inout) :: q
         end subroutine
      end interface
!
      interface
         subroutine PPGJPOST2L(part,cu,edges,npp,noff,ihole,qm,dt,nx,ny,&
     &idimp,npmax,nxv,nypmx,idps,ntmax,ipbc)
         implicit none
         integer, intent(in) :: npp, noff, nx, ny, idimp, npmax, idps
         integer, intent(in) :: ntmax, nxv, nypmx, ipbc
         real, intent(in) :: qm, dt
!        real, dimension(idimp,npmax), intent(inout) :: part
         real, dimension(*), intent(inout) :: part
!        real, dimension(3,nxv,nypmx), intent(inout) :: cu
         real, dimension(*), intent(inout) :: cu
!        real, dimension(idps), intent(in) :: edges
         real, dimension(*), intent(in) :: edges
!        integer, dimension(ntmax+1), intent(inout) :: ihole
         integer, dimension(*), intent(inout) :: ihole
         end subroutine
      end interface
!
      interface
         subroutine PPGMJPOST2L(part,amu,npp,noff,qm,idimp,npmax,nxv,   &
     &nypmx)
         implicit none
         integer, intent(in) :: npp, noff, idimp, npmax, nxv, nypmx
         real, intent(in) :: qm
!        real, dimension(idimp,npmax), intent(in) :: part
         real, dimension(*), intent(in) :: part
!        real, dimension(4,nxv,nypmx), intent(inout) :: amu
         real, dimension(*), intent(inout) :: amu
         end subroutine
      end interface
!
      interface
         subroutine PPGDJPOST2L(part,fxy,bxy,npp,noff,dcu,amu,qm,qbm,dt,&
     &idimp,npmax,nxv,nypmx)
         implicit none
         integer, intent(in) :: npp, noff, idimp, npmax, nxv, nypmx
         real, intent(in) :: qm, qbm, dt
!        real, dimension(idimp,npmax), intent(in) :: part
         real, dimension(*), intent(in) :: part
!        real, dimension(3,nxv,nypmx), intent(in) :: fxy, bxy
         real, dimension(*), intent(in) :: fxy, bxy
!        real, dimension(3,nxv,nypmx), intent(inout) :: dcu
         real, dimension(*), intent(inout) :: dcu
!        real, dimension(4,nxv,nypmx), intent(inout) :: amu
         real, dimension(*), intent(inout) :: amu
         end subroutine
      end interface
!
      interface
         subroutine PPGDCJPOST2L(part,fxy,bxy,npp,noff,cu,dcu,amu,qm,qbm&
     &,dt,idimp,npmax,nxv,nypmx)
         implicit none
         integer, intent(in) :: npp, noff, idimp, npmax, nxv, nypmx
         real, intent(in) :: qm, qbm, dt
!        real, dimension(idimp,npmax), intent(in) :: part
         real, dimension(*), intent(in) :: part
!        real, dimension(3,nxv,nypmx), intent(in) :: fxy, bxy
         real, dimension(*), intent(in) :: fxy, bxy
!        real, dimension(3,nxv,nypmx), intent(inout) :: cu, dcu
         real, dimension(*), intent(inout) :: cu, dcu
!        real, dimension(4,nxv,nypmx), intent(inout) :: amu
         real, dimension(*), intent(inout) :: amu
         end subroutine
      end interface
!
      interface
         subroutine PPDSORTP2YL(parta,partb,npic,npp,noff,nyp,idimp,    &
     &npmax,nypm1)
         implicit none
         integer, intent(in) :: npp, noff, nyp, idimp, npmax, nypm1
!        real, dimension(idimp,npmax), intent(in) :: parta
         real, dimension(*), intent(in) :: parta
!        real, dimension(idimp,npmax), intent(inout) :: partb
         real, dimension(*), intent(inout) :: partb
!        integer, dimension(nypm1), intent(inout) :: npic
         integer, dimension(*), intent(inout) :: npic
         end subroutine
      end interface
!
      interface
         subroutine PPCGUARD2XL(fxy,nyp,nx,ndim,nxe,nypmx)
         implicit none
         integer, intent(in) :: nyp, nx, ndim, nxe, nypmx
!        real, dimension(ndim,nxe,nypmx), intent(inout) :: fxy
         real, dimension(*), intent(inout) :: fxy
         end subroutine
      end interface
!
      interface
         subroutine PPAGUARD2XL(q,nyp,nx,nxe,nypmx)
         implicit none
         integer, intent(in) :: nyp, nx, nxe, nypmx
!        real, dimension(nxe,nypmx), intent(inout) :: q
         real, dimension(*), intent(inout) :: q
         end subroutine
      end interface
!
      interface
         subroutine PPACGUARD2XL(cu,nyp,nx,ndim,nxe,nypmx)
         implicit none
         integer, intent(in) :: nyp, nx, ndim, nxe, nypmx
!        real, dimension(ndim,nxe,nypmx), intent(inout) :: cu
         real, dimension(*), intent(inout) :: cu
         end subroutine
      end interface
!
      interface
         subroutine PPASCFGUARD2L(dcu,cus,nyp,q2m0,nx,nxe,nypmx)
         implicit none
         integer, intent(in) :: nyp, nx, nxe, nypmx
         real, intent(in) :: q2m0
!        real, dimension(3,nxe,nypmx), intent(inout) :: dcu
         real, dimension(*), intent(inout) :: dcu
!        real, dimension(3,nxe,nypmx), intent(in) :: cus
         real, dimension(*), intent(in) :: cus
         end subroutine
      end interface
!
      interface
         subroutine PPFWPMINMX2(qe,nyp,qbme,wpmax,wpmin,nx,nxe,nypmx)
         implicit none
         integer, intent(in) :: nyp, nx, nxe, nypmx
         real, intent(in) :: qbme
         real, intent(inout) :: wpmax, wpmin
!        real, dimension(nxe,nypmx), intent(in) :: qe
         real, dimension(*), intent(in) :: qe
         end subroutine
      end interface
!
      interface
         subroutine PPOIS23(q,fxy,isign,ffc,ax,ay,affp,we,nx,ny,kstrt,  &
     &nyv,kxp,nyhd)
         implicit none
         integer, intent(in) :: isign, nx, ny, kstrt, nyv, kxp, nyhd
         real, intent(in) :: ax, ay, affp
         real, intent(inout) :: we
!        complex, dimension(nyv,kxp), intent(in) :: q
         complex, dimension(*), intent(in) :: q
!        complex, dimension(3,nyv,kxp), intent(inout) :: fxy
         complex, dimension(*), intent(inout) :: fxy
!        complex, dimension(nyhd,kxp), intent(inout) :: ffc
         complex, dimension(*), intent(inout) :: ffc
         end subroutine
      end interface
!
      interface
         subroutine PPCUPERP2(cu,nx,ny,kstrt,nyv,kxp)
         implicit none
         integer, intent(in) :: nx, ny, kstrt, nyv, kxp
!        complex, dimension(3,nyv,kxp), intent(inout) :: cu
         complex, dimension(*), intent(inout) :: cu
         end subroutine
      end interface
!
      interface
         subroutine PPBBPOISP23(cu,bxy,ffc,ci,wm,nx,ny,kstrt,nyv,kxp,   &
     &nyhd)
         implicit none
         integer, intent(in) :: nx, ny, kstrt, nyv, kxp, nyhd
         real, intent(in) :: ci
         real, intent(inout) :: wm
!        complex, dimension(3,nyv,kxp), intent(in) :: cu
         complex, dimension(*), intent(in) :: cu
!        complex, dimension(3,nyv,kxp), intent(inout) :: bxy
         complex, dimension(*), intent(inout) :: bxy
!        complex, dimension(nyhd,kxp), intent(in) :: ffc
         complex, dimension(*), intent(in) :: ffc
         end subroutine
      end interface
!
      interface
         subroutine PPBADDEXT2(bxy,nyp,omx,omy,omz,nx,nxe,nypmx)
         implicit none
         integer, intent(in) :: nyp, nx, nxe, nypmx
         real, intent(in) :: omx, omy, omz
!        real, dimension(3,nxe,nypmx), intent(inout) :: bxy
         real, dimension(*), intent(inout) :: bxy
         end subroutine
      end interface
!
      interface
         subroutine PPDCUPERP23(dcu,amu,nx,ny,kstrt,nyv,kxp)
         implicit none
         integer, intent(in) :: nx, ny, kstrt, nyv, kxp
!        complex, dimension(3,nyv,kxp), intent(inout) :: dcu
         complex, dimension(*), intent(inout) :: dcu
!        complex, dimension(4,nyv,kxp), intent(in) :: amu
         complex, dimension(*), intent(in) :: amu
         end subroutine
      end interface
!
      interface
         subroutine PPADCUPERP23(dcu,amu,nx,ny,kstrt,nyv,kxp)
         implicit none
         integer, intent(in) :: nx, ny, kstrt, nyv, kxp
!        complex, dimension(3,nyv,kxp), intent(inout) :: dcu
         complex, dimension(*), intent(inout) :: dcu
!        complex, dimension(4,nyv,kxp), intent(in) :: amu
         complex, dimension(*), intent(in) :: amu
         end subroutine
      end interface
!
      interface
         subroutine PPEPOISP23(dcu,exy,isign,ffe,ax,ay,affp,wp0,ci,wf,nx&
     &,ny,kstrt,nyv,kxp,nyhd)
         implicit none
         integer, intent(in) :: isign, nx, ny, kstrt, nyv, kxp, nyhd
         real, intent(in) :: ax, ay, affp, wp0, ci
         real, intent(inout) :: wf
!        complex, dimension(3,nyv,kxp), intent(in) :: dcu
         complex, dimension(*), intent(in) :: dcu
!        complex, dimension(3,nyv,kxp), intent(inout) :: exy
         complex, dimension(*), intent(inout) :: exy
!        complex, dimension(nyhd,kxp), intent(inout) :: ffe
         complex, dimension(*), intent(inout) :: ffe
         end subroutine
      end interface
!
      interface
         subroutine PPADDVRFIELD2(a,b,c,ndim,nxe,nypmx)
         implicit none
         integer, intent(in) :: ndim, nxe, nypmx
!        real, dimension(ndim,nxe,nypmx), intent(inout) :: a
         real, dimension(*), intent(inout) :: a
!        real, dimension(ndim,nxe,nypmx), intent(in) :: b, c
         real, dimension(*), intent(in) :: b, c
         end subroutine
      end interface
!
      interface
         subroutine WPFFT2RINIT(mixup,sct,indx,indy,nxhyd,nxyhd)
         implicit none
         integer, intent(in) :: indx, indy, nxhyd, nxyhd
!        integer, dimension(nxhyd), intent(inout) :: mixup
         integer, dimension(*), intent(inout) :: mixup
!        complex, dimension(nxyhd), intent(inout) :: sct
         complex, dimension(*), intent(inout) :: sct
         end subroutine
      end interface
!
      interface
         subroutine WPPFFT2R(f,g,bs,br,isign,ntpose,mixup,sct,ttp,indx, &
     &indy,kstrt,nvp,nxvh,nyv,kxp,kyp,kypd,nxhyd,nxyhd)
         implicit none
         integer, intent(in) :: isign, ntpose, indx, indy, kstrt, nvp
         integer, intent(in) :: nxvh, nyv, kxp, kyp, kypd, nxhyd, nxyhd
         real, intent(inout) :: ttp
!        real, dimension(2*nxvh,kypd), intent(inout) :: f
         real, dimension(*), intent(inout) :: f
!        complex, dimension(nyv,kxp), intent(inout) :: g
         complex, dimension(*), intent(inout) :: g
!        complex, dimension(kxp,kyp), intent(inout) :: bs, br
         complex, dimension(*), intent(inout) :: bs, br
!        integer, dimension(nxhyd), intent(in) :: mixup
         integer, dimension(*), intent(in) :: mixup
!        complex, dimension(nxyhd), intent(in) :: sct
         complex, dimension(*), intent(in) :: sct
         end subroutine
      end interface
!
      interface
         subroutine WPPFFT2R3(f,g,bs,br,isign,ntpose,mixup,sct,ttp,indx,&
     &indy,kstrt,nvp,nxvh,nyv,kxp,kyp,kypd,nxhyd,nxyhd)
         implicit none
         integer, intent(in) :: isign, ntpose, indx, indy, kstrt, nvp
         integer, intent(in) :: nxvh, nyv, kxp, kyp, kypd, nxhyd, nxyhd
         real, intent(inout) :: ttp
!        real, dimension(3,2*nxvh,kypd), intent(inout) :: f
         real, dimension(*), intent(inout) :: f
!        complex, dimension(3,nyv,kxp), intent(inout) :: g
         complex, dimension(*), intent(inout) :: g
!        complex, dimension(3,kxp,kyp), intent(inout) :: bs, br
         complex, dimension(*), intent(inout) :: bs, br
!        integer, dimension(nxhyd), intent(in) :: mixup
         integer, dimension(*), intent(in) :: mixup
!        complex, dimension(nxyhd), intent(in) :: sct
         complex, dimension(*), intent(in) :: sct
         end subroutine
      end interface
!
      interface
         subroutine WPPFFT2RN(f,g,bs,br,ss,isign,ntpose,mixup,sct,ttp,  &
     &indx,indy,kstrt,nvp,nxvh,nyv,kxp,kyp,kypd,ndim,nxhyd,nxyhd)
         implicit none
         integer, intent(in) :: isign, ntpose, indx, indy, kstrt, nvp
         integer, intent(in) :: nxvh, nyv, kxp, kyp, kypd, ndim
         integer, intent(in) :: nxhyd, nxyhd
         real, intent(inout) :: ttp
!        real, dimension(ndim,2*nxvh,kypd), intent(inout) :: f
         real, dimension(*), intent(inout) :: f
!        complex, dimension(ndim,nyv,kxp), intent(inout) :: g
         complex, dimension(*), intent(inout) :: g
!        complex, dimension(ndim,kxp,kyp), intent(inout) :: bs, br
         complex, dimension(*), intent(inout) :: bs, br
!        complex, dimension(ndim,nxvh), intent(inout) :: ss
         complex, dimension(*), intent(inout) :: ss
!        integer, dimension(nxhyd), intent(in) :: mixup
         integer, dimension(*), intent(in) :: mixup
!        complex, dimension(nxyhd), intent(in) :: sct
         complex, dimension(*), intent(in) :: sct
         end subroutine
      end interface
!
      interface
         subroutine PPFFT2RXX(f,isign,mixup,sct,indx,indy,kstrt,kypi,   &
     &kypp,nxvh,kypd,nxhyd,nxyhd)
         implicit none
         integer, intent(in) :: isign, indx, indy, kstrt, kypi, kypp
         integer, intent(in) :: nxvh, kypd, nxhyd, nxyhd
!        real, dimension(2*nxvh,kypd), intent(inout) :: f
         real, dimension(*), intent(inout) :: f
!        integer, dimension(nxhyd), intent(in) :: mixup
         integer, dimension(*), intent(in) :: mixup
!        complex, dimension(nxyhd), intent(in) :: sct
         complex, dimension(*), intent(in) :: sct
         end subroutine
      end interface
!
      interface
         subroutine PPFFT2RXY(g,isign,mixup,sct,indx,indy,kstrt,kxpi,   &
     &kxpp,nyv,kxp,nxhyd,nxyhd)
         implicit none
         integer, intent(in) :: isign, indx, indy, kstrt, kxpi, kxpp
         integer, intent(in) :: nyv, kxp, nxhyd, nxyhd
!        complex, dimension(nyv,kxp), intent(inout) :: g
         complex, dimension(*), intent(inout) :: g
!        integer, dimension(nxhyd), intent(in) :: mixup
         integer, dimension(*), intent(in) :: mixup
!        complex, dimension(nxyhd), intent(in) :: sct
         complex, dimension(*), intent(in) :: sct
         end subroutine
      end interface
!
      interface
         subroutine PPFFT2R3XX(f,isign,mixup,sct,indx,indy,kstrt,kypi,  &
     &kypp,nxvh,kypd,nxhyd,nxyhd)
         implicit none
         integer, intent(in) :: isign, indx, indy, kstrt, kypi, kypp
         integer, intent(in) :: nxvh, kypd, nxhyd, nxyhd
!        real, dimension(3,2*nxvh,kypd), intent(inout) :: f
         real, dimension(*), intent(inout) :: f
!        integer, dimension(nxhyd), intent(in) :: mixup
         integer, dimension(*), intent(in) :: mixup
!        complex, dimension(nxyhd), intent(in) :: sct
         complex, dimension(*), intent(in) :: sct
         end subroutine
      end interface
!
      interface
         subroutine PPFFT2R3XY(g,isign,mixup,sct,indx,indy,kstrt,kxpi,  &
     &kxpp,nyv,kxp,nxhyd,nxyhd)
         implicit none
         integer, intent(in) :: isign, indx, indy, kstrt, kxpi, kxpp
         integer, intent(in) :: nyv, kxp, nxhyd, nxyhd
!        complex, dimension(3,nyv,kxp), intent(inout) :: g
         complex, dimension(*), intent(inout) :: g
!        integer, dimension(nxhyd), intent(in) :: mixup
         integer, dimension(*), intent(in) :: mixup
!        complex, dimension(nxyhd), intent(in) :: sct
         complex, dimension(*), intent(in) :: sct
         end subroutine
      end interface
!
      interface
         subroutine PPFFT2RNXX(f,ss,isign,mixup,sct,indx,indy,kstrt,kypi&
     &,kypp,nxvh,kypd,ndim,nxhyd,nxyhd)
         implicit none
         integer, intent(in) :: isign, indx, indy, kstrt, kypi, kypp
         integer, intent(in) :: nxvh, kypd, ndim, nxhyd, nxyhd
!        real, dimension(ndim,2*nxvh,kypd), intent(inout) :: f
         real, dimension(*), intent(inout) :: f
!        complex, dimension(ndim,nxvh), intent(inout) :: ss
         complex, dimension(*), intent(inout) :: ss
!        integer, dimension(nxhyd), intent(in) :: mixup
         integer, dimension(*), intent(in) :: mixup
!        complex, dimension(nxyhd), intent(in) :: sct
         complex, dimension(*), intent(in) :: sct
         end subroutine
      end interface
!
      interface
         subroutine PPFFT2RNXY(g,isign,mixup,sct,indx,indy,kstrt,kxpi,  &
     &kxpp,nyv,kxp,ndim,nxhyd,nxyhd)
         implicit none
         integer, intent(in) :: isign, indx, indy, kstrt, kxpi, kxpp
         integer, intent(in) :: nyv, kxp, ndim, nxhyd, nxyhd
!        complex, dimension(ndim,nyv,kxp), intent(inout) :: g
         complex, dimension(*), intent(inout) :: g
!        integer, dimension(nxhyd), intent(in) :: mixup
         integer, dimension(*), intent(in) :: mixup
!        complex, dimension(nxyhd), intent(in) :: sct
         complex, dimension(*), intent(in) :: sct
         end subroutine
      end interface
!
      interface
         subroutine PPSWAPC2N(f,s,isign,nxh,kypi,kypt,nxvh,kypd,ndim)
         implicit none
         integer, intent(in) :: isign, nxh, kypi, kypt, nxvh, kypd, ndim
!        real, dimension(ndim,2*nxvh,kypd), intent(inout) :: f
         real, dimension(*), intent(inout) :: f
!        complex, dimension(ndim*nxvh), intent(inout) :: s
         complex, dimension(*), intent(inout) :: s
         end subroutine
      end interface
!
      interface
         function ranorm()
         implicit none
         double precision :: ranorm
         end function
      end interface
!
!-----------------------------------------------------------------------
! Interface file for pplib2.f
!     module pplib2_h
!     implicit none
!
      interface
         subroutine PPINIT2(idproc,nvp)
         implicit none
         integer, intent(inout) :: idproc, nvp
         end subroutine
      end interface
!
      interface
         subroutine PPEXIT
         implicit none
         end subroutine
      end interface
!
      interface
         subroutine PPABORT
         implicit none
         end subroutine
      end interface
!
      interface
         subroutine PWTIMERA(icntrl,time,dtime)
         implicit none
         integer, intent(in) :: icntrl
         real, intent(inout) :: time
         double precision, intent(inout) :: dtime
         end subroutine
      end interface
!
      interface
         subroutine PPSUM(f,g,nxp)
         implicit none
         integer, intent(in) :: nxp
!        real, dimension(nxp), intent(inout) :: f, g
         real, dimension(*), intent(inout) :: f, g
         end subroutine
      end interface
!
      interface
         subroutine PPDSUM(f,g,nxp)
         implicit none
         integer, intent(in) :: nxp
!        double precision, dimension(nxp), intent(inout) :: f, g
         double precision, dimension(*), intent(inout) :: f, g
         end subroutine
      end interface
!
      interface
         subroutine PPIMAX(if,ig,nxp)
         implicit none
         integer, intent(in) :: nxp
!        integer, dimension(nxp), intent(inout) :: if, ig
         integer, dimension(*), intent(inout) :: if, ig
         end subroutine
      end interface
!
      interface
         subroutine PPDMAX(f,g,nxp)
         implicit none
         integer, intent(in) :: nxp
!        double precision, dimension(nxp), intent(inout) :: f, g
         double precision, dimension(*), intent(inout) :: f, g
         end subroutine
      end interface
!
      interface
         subroutine PPNCGUARD2L(f,nyp,kstrt,nvp,nxv,nypmx)
         implicit none
         integer, intent(in) :: nyp, kstrt, nvp, nxv, nypmx
!        real, dimension(nxv,nypmx), intent(inout) :: f
         real, dimension(*), intent(inout) :: f
         end subroutine
      end interface
!
      interface
         subroutine PPNAGUARD2L(f,scr,nyp,nx,kstrt,nvp,nxv,nypmx)
         implicit none
         integer, intent(in) :: nyp, kstrt, nvp, nx, nxv, nypmx
!        real, dimension(nxv,nypmx), intent(inout) :: f
         real, dimension(*), intent(inout) :: f
!        real, dimension(nxv), intent(inout) :: scr
         real, dimension(*), intent(inout) :: scr
         end subroutine
      end interface
!
      interface
         subroutine PPNACGUARD2L(f,scr,nyp,nx,ndim,kstrt,nvp,nxv,nypmx)
         implicit none
         integer, intent(in) :: nyp, ndim, kstrt, nvp, nx, nxv, nypmx
!        real, dimension(ndim,nxv,nypmx), intent(inout) :: f
         real, dimension(*), intent(inout) :: f
!        real, dimension(ndim,nxv), intent(inout) :: scr
         real, dimension(*), intent(inout) :: scr
         end subroutine
      end interface
!
      interface
         subroutine PPTPOSE(f,g,s,t,nx,ny,kxp,kyp,kstrt,nvp,nxv,nyv,kxpd&
     &,kypd)
         implicit none
         integer, intent(in) :: nx, ny, kxp, kyp, kstrt, nvp, nxv, nyv
         integer, intent(in) :: kxpd, kypd
!        real, dimension(2*nxv,kypd), intent(in) :: f
         real, dimension(*), intent(in) :: f
!        complex, dimension(nyv,kxpd), intent(inout) :: g
         complex, dimension(*), intent(inout) :: g
!        complex, dimension(kxp*kyp), intent(inout) :: s, t
         complex, dimension(*), intent(inout) :: s, t
         end subroutine
      end interface
!
      interface
         subroutine PPNTPOSE(f,g,s,t,nx,ny,kxp,kyp,kstrt,nvp,ndim,nxv,  &
     &nyv,kxpd,kypd)
         implicit none
         integer, intent(in) :: nx, ny, kxp, kyp, kstrt, nvp, ndim
         integer, intent(in) :: nxv, nyv, kxpd, kypd
!        real, dimension(ndim,2*nxv,kypd), intent(in) :: f
         real, dimension(*), intent(in) :: f
!        complex, dimension(ndim,nyv,kxpd), intent(inout) :: g
         complex, dimension(*), intent(inout) :: g
!        complex, dimension(ndim,kxp*kyp), intent(inout) :: s, t
         complex, dimension(*), intent(inout) :: s, t
         end subroutine
      end interface
!
      interface
         subroutine PPMOVE2(part,edges,npp,sbufr,sbufl,rbufr,rbufl,ihole&
     &,ny,kstrt,nvp,idimp,npmax,idps,nbmax,ntmax,info)
         implicit none
         integer, intent(in) :: ny, kstrt, nvp, idimp, npmax, idps
         integer, intent(in) :: nbmax, ntmax
         integer, intent(inout) :: npp
!        real, dimension(idimp,npmax), intent(inout) :: part
         real, dimension(*), intent(inout) :: part
!        real, dimension(idps), intent(in) :: edges
         real, dimension(*), intent(in) :: edges
!        real, dimension(idimp,nbmax), intent(inout) :: sbufr, sbufl
         real, dimension(*), intent(inout) :: sbufr, sbufl
!        real, dimension(idimp,nbmax), intent(inout) :: rbufr, rbufl
         real, dimension(*), intent(inout) :: rbufr, rbufl
!        integer, dimension(ntmax+1) , intent(inout):: ihole
         integer, dimension(*) , intent(inout):: ihole
!        integer, dimension(5), intent(inout) :: info
         integer, dimension(*), intent(inout) :: info
         end subroutine
      end interface
!
!     end module
      end

