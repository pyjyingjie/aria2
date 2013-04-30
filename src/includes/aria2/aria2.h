/* <!-- copyright */
/*
 * aria2 - The high speed download utility
 *
 * Copyright (C) 2013 Tatsuhiro Tsujikawa
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */
/* copyright --> */
#ifndef ARIA2_H
#define ARIA2_H

#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>

#include <string>
#include <vector>

// Libaria2: The aim of this library is provide same functionality
// available in RPC methods. The function signatures are not
// necessarily the same, because we can take advantage of the direct,
// no latency, access to the aria2 core.
//
// Therefore, this library is not meant to be the fine-grained,
// customizable, complete HTTP/FTP/BitTorrent library. If you are
// looking for such library for HTTP/FTP access, consider libcurl.

namespace aria2 {

struct Session;

// Initializes the global data. It also initializes
// underlying libraries libaria2 depends on. This function returns 0
// if it succeeds, or -1.
//
// Call this function only once before calling any other API functions.
int libraryInit();

// Releases the global data. This function returns 0 if
// it succeeds, or -1.
//
// Call this function only once at the end of the application.
int libraryDeinit();

// type of GID
typedef uint64_t A2Gid;

// type of Key/Value pairs
typedef std::vector<std::pair<std::string, std::string> > KeyVals;

// Creates new Session object using the |options| as additional
// parameters. The |options| is treated as if they are specified in
// command-line to aria2c(1). This function returns the pointer to the
// newly created Session object if it succeeds, or NULL.
//
// Please note that only one Session object can be created per
// process.
Session* sessionNew(const KeyVals& options);

// Performs post-download action, including saving sessions etc and
// destroys the |session| object, releasing the allocated resources
// for it. This function returns the last error code and it is the
// equivalent to the exit status of aria2c(1).
int sessionFinal(Session* session);

enum RUN_MODE {
  RUN_DEFAULT,
  RUN_ONCE
};

// Performs event polling and actions for them. If the |mode| is
// RUN_DEFAULT, this function returns when no downloads are left to be
// processed. In this case, this function returns 0.
//
// If the |mode| is RUN_ONCE, this function returns after one event
// polling. In the current implementation, event polling timeouts in 1
// second, so this function returns at most 1 second. On return, when
// no downloads are left to be processed, this function returns
// 0. Otherwise, returns 1, indicating that the caller must call this
// function one or more time to complete downloads.
int run(Session* session, RUN_MODE mode);

// Returns textual representation of the |gid|.
std::string gidToHex(const A2Gid& gid);
// Returns GID converted from the textual representation |hex|.
A2Gid hexToGid(const std::string& hex);
// Returns true if the |gid| is invalid.
bool isNull(const A2Gid& gid);

// This method adds new HTTP(S)/FTP/BitTorrent Magnet URI.  On
// successful return, the |gid| includes the GID of newly added
// download.  The |uris| includes URI to be downloaded.  For
// BitTorrent Magnet URI, the |uris| must have only one element and it
// should be BitTorrent Magnet URI. URIs in uris must point to the
// same file. If you mix other URIs which point to another file, aria2
// does not complain but download may fail. The |options| is a pair of
// option name and value. If the |position| is not negative integer,
// the new download is inserted at position in the waiting queue. If
// the |position| is negative or the |position| is larger than the
// size of the queue, it is appended at the end of the queue.  This
// function returns 0 if it succeeds, or -1.
int addUri(Session* session,
           A2Gid& gid,
           const std::vector<std::string>& uris,
           const KeyVals& options,
           int position = -1);

// Returns the array of active download GID.
std::vector<A2Gid> getActiveDownload(Session* session);

enum DOWNLOAD_STATUS {
  DOWNLOAD_ACTIVE,
  DOWNLOAD_WAITING,
  DOWNLOAD_PAUSED,
  DOWNLOAD_COMPLETE,
  DOWNLOAD_ERROR,
  DOWNLOAD_REMOVED
};

struct DownloadHandle;

// Returns handle for the download denoted by the |gid|. The caller
// can retrieve various information of the download via returned
// handle. The lifetime of the returned handle is before the next call
// of run() or sessionFinal(). This function returns NULL if no
// download denoted by the |gid| is present. The caller must call
// deleteDownloadHandle() to delete the acquired handle.
DownloadHandle* getDownloadHandle(Session* session, const A2Gid& gid);

// Deallocates the |dh|. Calling this function with NULL is safe.
void deleteDownloadHandle(DownloadHandle* dh);

DOWNLOAD_STATUS downloadGetStatus(DownloadHandle* dh);
int64_t downloadGetTotalLength(DownloadHandle* dh);
int64_t downloadGetCompletedLength(DownloadHandle* dh);
int64_t downloadGetUploadLength(DownloadHandle* dh);
std::string downloadGetBitfield(DownloadHandle* dh);
int downloadGetDownloadSpeed(DownloadHandle* dh);
int downloadGetUploadSpeed(DownloadHandle* dh);
size_t downloadGetNumPieces(DownloadHandle* dh);
int downloadGetConnections(DownloadHandle* dh);
int downloadGetErrorCode(DownloadHandle* dh);
const std::vector<A2Gid>& downloadGetFollowedBy(DownloadHandle* dh);
A2Gid downloadGetBelongsTo(DownloadHandle* dh);
const std::string& downloadGetDir(DownloadHandle* dh);

} // namespace aria2

#endif // ARIA2_H