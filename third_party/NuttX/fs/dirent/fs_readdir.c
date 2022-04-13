/****************************************************************************
 * fs/dirent/fs_readdir.c
 *
 *   Copyright (C) 2007-2009, 2011, 2017-2018 Gregory Nutt. All rights
 *     reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "vfs_config.h"

#include "string.h"
#include "dirent.h"
#include "errno.h"
#include "unistd.h"

#include "fs/fs.h"
#include "fs/dirent_fs.h"

#include "inode/inode.h"
#include "sys/statfs.h"
#include "user_copy.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: readpseudodir
 ****************************************************************************/

static inline int readpseudodir(struct fs_dirent_s *idir)
{
  FAR struct inode *prev;
  int               ret;

  /* Check if we are at the end of the list */

  if (!idir->u.pseudo.fd_next)
    {
      /* End of file and error conditions are not distinguishable with
       * readdir.  Here we return -ENOENT to signal the end of the
       * directory.
       */

      return -ENOENT;
    }

  /* Copy the inode name into the dirent structure */
  ret = strncpy_s(idir->fd_dir[0].d_name, NAME_MAX + 1, idir->u.pseudo.fd_next->i_name, NAME_MAX);
  if (ret != EOK)
    {
      return -ENAMETOOLONG;
    }

  /* If the node has file operations, we will say that it is a file. */

  idir->fd_dir[0].d_type = 0;
  if (idir->u.pseudo.fd_next->u.i_ops)
    {
#ifndef CONFIG_DISABLE_MOUNTPOINT
      if (INODE_IS_BLOCK(idir->u.pseudo.fd_next))
        {
           idir->fd_dir[0].d_type |= DT_BLK;
        }
      if (INODE_IS_MOUNTPT(idir->u.pseudo.fd_next))
        {
           idir->fd_dir[0].d_type |= DT_DIR;
        }
      else
#endif
        {
           idir->fd_dir[0].d_type |= DT_CHR;
        }
    }

  /* If the node has child node(s) or no operations, then we will say that
   * it is a directory rather than a special file.  NOTE: that the node can
   * be both!
   */

  if (idir->u.pseudo.fd_next->i_child || !idir->u.pseudo.fd_next->u.i_ops)
    {
      idir->fd_dir[0].d_type |= DT_DIR;
    }

  /* Now get the inode to vist next time that readdir() is called */

  inode_semtake();

  prev                   = idir->u.pseudo.fd_next;
  idir->u.pseudo.fd_next = prev->i_peer; /* The next node to visit */

  if (idir->u.pseudo.fd_next)
    {
      /* Increment the reference count on this next node */

      idir->u.pseudo.fd_next->i_crefs++;
    }

  inode_semgive();

  if (prev)
    {
      inode_release(prev);
    }

  return OK;
}

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: handlezpfsdir
 * Description:
 *   The handlezpfsdir() function handle with the inode with zpfs magic
 *
 * Input Parameters:
 *   idir -- the pointer of one dir
 *
 * Returned Value:
 *   none
 ****************************************************************************/
#ifdef LOSCFG_FS_ZPFS
static inline void handlezpfsdir(struct fs_dirent_s *idir)
{
  struct statfs buf;
  int           ret;
  /* Maybe there are two or more zpfs nodes together, so we need one loop */
  do
    {
      if (!idir->u.pseudo.fd_next || !INODE_IS_MOUNTPT(idir->u.pseudo.fd_next) ||
          !idir->u.pseudo.fd_next->u.i_mops || !idir->u.pseudo.fd_next->u.i_mops->statfs)
        {
          return;
        }
      ret = idir->u.pseudo.fd_next->u.i_mops->statfs(idir->u.pseudo.fd_next, &buf);
      if (ret != OK || buf.f_type != ZPFS_MAGIC)
        {
          return;
        }
      if (readpseudodir(idir) == OK)
        {
          idir->fd_position++;
        }
    } while (1);
}
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: do_readdir
 *
 * Description:
 *   The do_readdir() function returns a pointer to a dirent structure
 *   representing the next directory entry in the directory stream pointed
 *   to by dir.  It returns NULL on reaching the end-of-file or if an error
 *   occurred.
 *
 * Input Parameters:
 *   dirp -- An instance of type DIR created by a previous call to opendir();
 *
 * Returned Value:
 *   The do_readdir() function returns a pointer to a dirent structure, or NULL
 *   if an error occurs or end-of-file is reached.  On error, errno is set
 *   appropriately.
 *
 *   EBADF   - Invalid directory stream descriptor dir
 *
 ****************************************************************************/
static struct dirent *__readdir(DIR *dirp, int *lencnt)
{
  FAR struct fs_dirent_s *idir = (struct fs_dirent_s *)dirp;
  struct inode *inode_ptr = NULL;
  int ret = 0;
  int file_cnt = 0;
  /* Verify that we were provided with a valid directory structure */

  if (!idir || idir->fd_status != DIRENT_MAGIC)
    {
      ret = EBADF;
      goto errout;
    }

  /* A special case is when we enumerate an "empty", unused inode.  That is
   * an inode in the pseudo-filesystem that has no operations and no children.
   * This is a "dangling" directory entry that has lost its children.
   */

  inode_ptr = idir->fd_root;
  if (inode_ptr == NULL)
    {
      /* End of file and error conditions are not distinguishable
       * with readdir.  We return NULL to signal either case.
       */

      ret = ENOENT;
      goto errout;
    }

  /* The way we handle the readdir depends on the type of inode
   * that we are dealing with.
   */
  if (DIRENT_ISPSEUDONODE(idir->fd_flags))
    {
#ifdef LOSCFG_FS_ZPFS
      /* if the current node has the zpfs magic, we continue to skip the node */
      handlezpfsdir(idir);
#endif
      /* The node is part of the root pseudo file system */

      ret = readpseudodir(idir);
      if (ret == OK)
        {
          idir->fd_position++;
          idir->fd_dir[0].d_off = idir->fd_position;
          idir->fd_dir[0].d_reclen = (uint16_t)sizeof(struct dirent);
          *lencnt = sizeof(struct dirent);
          return &(idir->fd_dir[0]);
        }
    }

#ifndef CONFIG_DISABLE_MOUNTPOINT
  if (INODE_IS_MOUNTPT(inode_ptr))
    {
      /* The node is a file system mointpoint. Verify that the mountpoint
       * supports the readdir() method
       */
      if (!inode_ptr->u.i_mops || !inode_ptr->u.i_mops->readdir)
        {
          ret = EACCES;
          goto errout;
        }

      /* Perform the readdir() operation */
#ifdef LOSCFG_ENABLE_READ_BUFFER
      idir->read_cnt = MAX_DIRENT_NUM;
#else
      idir->read_cnt = 1;
#endif
      file_cnt = inode_ptr->u.i_mops->readdir(inode_ptr, idir);
      if (file_cnt > 0)
        {
          *lencnt = file_cnt * sizeof(struct dirent);
          return &(idir->fd_dir[0]);
        }
    }
#endif

errout:
  if (ret != OK)
    {
      if (ret < 0)
        {
          ret = -ret;
        }
      set_errno(ret);
    }
  else if (file_cnt <= 0)
    {
      set_errno(ENOENT);
    }
  return (struct dirent *)NULL;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: readdir
 *
 * Description:
 *   The readdir() function returns a pointer to a dirent structure
 *   representing the next directory entry in the directory stream pointed
 *   to by dir.  It returns NULL on reaching the end-of-file or if an error
 *   occurred.
 *
 * Inputs:
 *   dirp -- An instance of type DIR created by a previous call to opendir();
 *
 * Return:
 *   The readdir() function returns a pointer to a dirent structure, or NULL
 *   if an error occurs or end-of-file is reached.  On error, errno is set
 *   appropriately.
 *
 *   EBADF   - Invalid directory stream descriptor dir
 *
 ****************************************************************************/

FAR struct dirent *readdir(DIR *dirp)
{
  int ret;
  int old_err = get_errno();
  int lencnt = 0;
  struct dirent *de = NULL;
  FAR struct fs_dirent_s *idir = (struct fs_dirent_s *)dirp;
  int dirlen;

#ifdef LOSCFG_ENABLE_READ_BUFFER
  dirlen = MAX_DIRENT_NUM;
#else
  dirlen = 1;
#endif
  if (idir->cur_pos != 0 && idir->cur_pos < dirlen && idir->cur_pos < idir->end_pos)
    {
      de = &(idir->fd_dir[idir->cur_pos]);
      if (idir->cur_pos == dirlen)
        {
          idir->cur_pos = 0;
        }
      idir->cur_pos++;
      return de;
    } else {
      de = __readdir(dirp, &lencnt);
      idir->end_pos = lencnt / sizeof(struct dirent);
      idir->cur_pos = 1;

      if (de == NULL)
        {
          idir->cur_pos = 0;
          ret = get_errno();
          /* Special case: ret = -ENOENT is end of file */

          if (ret == ENOENT)
            {
              set_errno(old_err);
            }
        }
    }
  return de;
}

/* readdir syscall routine */

int do_readdir(int fd, struct dirent **de, unsigned int count)
{
  struct dirent *de_src = NULL;
  int lencnt = 0;
  /* Did we get a valid file descriptor? */

#if CONFIG_NFILE_DESCRIPTORS > 0
  FAR struct file *filep = NULL;

  if (de == NULL)
    {
      return -EINVAL;
    }

  if ((unsigned int)fd >= CONFIG_NFILE_DESCRIPTORS)
    {
      return -EBADF;
    }
  else
    {
      /* The descriptor is in a valid range to file descriptor... do the
       * read.  First, get the file structure.
       */

      int ret = fs_getfilep(fd, &filep);
      if (ret < 0)
        {
          /* The errno value has already been set */
          return -get_errno();
        }

      /* Then let do_readdir do all of the work */
      de_src = __readdir(filep->f_dir, &lencnt);
      if (de_src == NULL)
        {
          /* Special case: ret = -ENOENT is end of file */
          return -get_errno();
        }
      *de = de_src;

      lencnt = (lencnt != 0) ? lencnt : sizeof(*de_src);
      return lencnt;
    }
#endif

  return 0;
}
