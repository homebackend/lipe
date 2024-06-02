/*
 * 
 * Copyright (C) 2004 Neeraj Jakhar <neerajcd@iitk.ac.in>
 *
 * 
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation; either 
 * version 2 of the License, or (at your option) any later 
 * version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 */

#ifndef I386TYPES_H
#define I386TYPES_H

/* Do not allow '<' and '>' in the strings as they interfere gtk parsing. */

static char* i386_type_names[]=
{
	"List of Partition Types",
	"0x00: Empty",
	"0x01: DOS FAT12",
	"0x02: XENIX root",
	"0x03: XENIX usr",
	"0x04: DOS 3.3+ FAT16 (less than 32M)",
	"0x05: DOS 3.3+ Extended",
	"0x06: DOS 3.31+ FAT16 (greater than 32M)",
	"0x07: OS/2 IFS (HPFS)/Windows NT NTFS/QNX 2.x",
	"0x08: AIX/QNX 1.x & 2.x",
	"0x09: AIX bootable or Coherant filesystem",
	"0x0a: OS/2 Boot Manager or Coherant swap/OPUS",
	"0x0b: Win95 OSR2 FAT32",
	"0x0c: Win95 OSR2 FAT32 (LBA)",
	"0x0e: Win95 FAT16 (LBA)",
	"0x0f: Win95 Extended (LBA)",
	"0x10: OPUS",
	"0x11: Hidden DOS FAT12",
	"0x12: Compaq diagnostics",
	"0x14: Hidden DOS 3.3+ FAT16 (less than 32M)",
	"0x16: Hidden DOS 3.31+ FAT16 (greater than 32M)",
	"0x17: Hidden HPFS or NTFS",
	"0x18: AST SmartSleep",
	"0x1b: Hidden Win95 OSR2 FAT32",
	"0x1c: Hidden Win95 OSR2 FAT32 (LBA)",
	"0x1e: Hidden Win95 FAT16 (LBA)",
	"0x24: NEC DOS 3.x",
	"0x32: NOS",
	"0x35: JFS on OS/2 or eCS",
	"0x39: Plan 9 or THEOS ver 4 spanned partition",
	"0x3a: THEOS ver 4 4gb partition",
	"0x3b: THEOS ver 4 extended partition",
	"0x3c: PartitionMagic recovery partition",
	"0x40: Venix 80286",
	"0x41: Linux-MINIX (DRDOS) or Personal RISC Boot or PPC PReP Boot",
	"0x42: Linux swap (DRDOS) or SFS or Windows 2000",
	"0x43: Linux native (sharing disk with DRDOS)",
	"0x45: EUMEL/ELAN or Boot-US boot manager/Priam",
	"0x46: EUMEL/Elan",
	"0x47: EUMEL/Elan",
	"0x48: EUMEL/Elan",
	"0x4a: AdaOS Aqaauila (Default) or ALFS/THIN lightweight filesystem for DOS",
	"0x4c: Oberon partition",
	"0x4d: QNX4.x",
	"0x4e: QNX4.x 2nd part",
	"0x4f: QNX4.x 3rd part or Oberon partition",
	"0x50: OnTrack Disk Manager (older versions) RO or Lynx RTOS or Native Oberon (alt)",
	"0x51: OnTrack Disk Manager RW (DM6 Aux1) or Novell",
	"0x52: CP/M or Microport SysV/AT",
	"0x53: OnTrack Disk Manager 6.0 Aux3",
	"0x54: OnTrack Disk Manager 6.0 Dynamic Drive Overlay",
	"0x55: EZ-Drive",
	"0x56: Golden Bow VFeature Partitioned Volume or DM converted to EZ-BIOS",
	"0x57: DrivePro or VNDI Patition",
	"0x5c: Priam Edisk",
	"0x61: SpeedStor",
	"0x63: GNU HURD or Mach or Unix System V (SCO, ISC Unix, Unixware, ...)",
	"0x64: Novell Netware 286, 2.xx or PC-ARMOUR protected partition",
	"0x65: Novell Netware 386, 3.xx/4.xx",
	"0x66: Novell Netware SMS Partition",
	"0x67: Novell",
	"0x68: Novell",
	"0x69: Novell Netware 5+, Novell Netware NSS Partition",
	"0x70: DiskSecure Multi-Boot",
	"0x74: Scramdisk partition",
	"0x75: IBM PC/IX",
	"0x77: M2FS/M2CS partition or VNDI Partition",
	"0x78: XOSL FS",
	"0x80: Minix 1.4a and earlier",
	"0x81: Minix 1.4b and later or old Linux or Mitac disk manager",
	"0x82: Linux swap or Solaris x86 or Prime",
	"0x83: Linux native (usually ext2fs)",
	"0x84: OS/2 hidden C: drive or Hibernation partition",
	"0x85: Linux extended partition",
	"0x86: NTFS volume set or Old Linux RAID partition superblock",
	"0x87: NTFS volume set",
	"0x8a: Linux Kernel Partition (used by AiR-BOOT)",
	"0x8b: Legacy Fault Tolerant FAT32 volume",
	"0x8c: Legacy Fault Tolerant FAT32 volume using BIOS extd INT 13h",
	"0x8d: Free FDISK hidden Primary DOS FAT12 partitition",
	"0x8e: Linux LVM (Logical Volume Manager)",
	"0x90: Free FDISK hidden Primary DOS FAT16 partitition",
	"0x91: Free FDISK hidden DOS extended partitition",
	"0x92: Free FDISK hidden Primary DOS large FAT16 partitition",
	"0x93: Hidden Linux native partition or Amoeba",
	"0x94: Amoeba BBT (bad block table)",
	"0x95: MIT EXOPC native partitions",
	"0x97: Free FDISK hidden Primary DOS FAT32 partitition",
	"0x98: Free FDISK hidden Primary DOS FAT32 partitition (LBA)",
	"0x99: DCE376 logical drive",
	"0x9a: Free FDISK hidden Primary DOS FAT16 partitition (LBA)",
	"0x9b: Free FDISK hidden DOS extended partitition (LBA)",
	"0x9f: BSD/OS",	
	"0xa0: IBM Thinkpad hibernation",
	"0xa1: IBM Thinkpad hibernation or HP Volume Expansion (SpeedStor variant)",
	"0xa5: BSD/386, 386BSD, NetBSD, FreeBSD",
	"0xa6: OpenBSD",
	"0xa7: NeXTSTEP",
	"0xa8: Mac OS-X or Darwin UFS",
	"0xa9: NetBSD",
	"0xaa: Olivetti Fat 12 1.44Mb Service Partition",
	"0xab: Mac OS-X Boot partition or Darwin boot",
	"0xb7: BSDI fs",
	"0xb8: BSDI swap",
	"0xbb: Boot Wizard hidden",
	"0xbe: Solaris boot",
	"0xc1: DRDOS/sec (FAT-12)",
	"0xc4: DRDOS/sec (FAT-16 (less than 32M))",
	"0xc6: DRDOS/sec (FAT-16)",
	"0xc7: Syrinx",
	"0xda: Non-FS data",
	"0xdb: CP/M or Concurrent CP/M or Concurrent DOS or CTOS",
	"0xde: Dell PowerEdge Server Utilities",
	"0xdf: BootIt EMBRM",
	"0xe1: DOS access or SpeedStor 12-bit FAT extended partition",
	"0xe3: DOS R/O or SpeedStor",
	"0xe4: SpeedStor 16-bit FAT extended partition (less than 1024 cyl)",
	"0xeb: BeOS fs",
	"0xee: Intel EFI GPT (GUID Partition Table)",
	"0xef: Intel EFI System Partition (FAT-12/16/32)",
	"0xf0: Linux/PA-RISC boot loader",
	"0xf1: SpeedStor",
	"0xf4: SpeedStor large partition",
	"0xf2: DOS 3.3+ secondary",
	"0xfd: Linux raid autodetect",
	"0xfe: LANstep or SpeedStor (greater than 1024 cyl)",
	"0xff: Xenix BBT (Bad Block Table)",
	0
};


#endif /* I386TYPES_H */
