/* <!-- copyright */
/*
 * aria2 - The high speed download utility
 *
 * Copyright (C) 2006 Tatsuhiro Tsujikawa
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
#ifndef _D_UNKNOWN_LENGTH_PIECE_STORAGE_H_
#define _D_UNKNOWN_LENGTH_PIECE_STORAGE_H_

#include "PieceStorage.h"

namespace aria2 {

class Option;
class DownloadContext;
class DiskWriterFactory;
class DirectDiskAdaptor;

class UnknownLengthPieceStorage:public PieceStorage {
private:
  SharedHandle<DownloadContext> _downloadContext;

  const Option* _option;
  
  SharedHandle<DirectDiskAdaptor> _diskAdaptor;

  SharedHandle<DiskWriterFactory> _diskWriterFactory;

  uint64_t _totalLength;

  bool _downloadFinished;

  SharedHandle<Piece> _piece;
public:
  UnknownLengthPieceStorage(const SharedHandle<DownloadContext>& downloadContext,
			    const Option* option);

  virtual ~UnknownLengthPieceStorage();

  /**
   * Returns true if the peer has a piece that localhost doesn't have.
   * Otherwise returns false.
   */
  virtual bool hasMissingPiece(const SharedHandle<Peer>& peer);

  /**
   * Returns a piece that the peer has but localhost doesn't.
   * The piece will be marked "used" status in order to prevent other command
   * from get the same piece. But in end game mode, same piece may be returned
   * to several commands.
   */
  virtual SharedHandle<Piece> getMissingPiece(const SharedHandle<Peer>& peer);

  /**
   * Returns a piece that the peer has but localhost doesn't.
   * Only pieces that declared as "fast" are returned.
   * The piece will be marked "used" status in order to prevent other command
   * from get the same piece. But in end game mode, same piece may be returned
   * to several commands.
   */
  virtual SharedHandle<Piece> getMissingFastPiece(const SharedHandle<Peer>& peer);

  /**
   * Returns a missing piece if available. Otherwise returns 0;
   */
  virtual SharedHandle<Piece> getMissingPiece();

  /**
   * Returns a missing piece whose index is index.
   * If a piece whose index is index is already acquired or currently used,
   * then returns 0.
   * Also returns 0 if any of missing piece is not available.
   */
  virtual SharedHandle<Piece> getMissingPiece(size_t index);

  /**
   * Returns the piece denoted by index.
   * No status of the piece is changed in this method.
   */
  virtual SharedHandle<Piece> getPiece(size_t index);

  /**
   * Tells that the download of the specfied piece completes.
   */
  virtual void completePiece(const SharedHandle<Piece>& piece);

  /**
   * Tells that the download of the specified piece is canceled.
   */
  virtual void cancelPiece(const SharedHandle<Piece>& piece);

  /**
   * Returns true if the specified piece is already downloaded.
   * Otherwise returns false.
   */
  virtual bool hasPiece(size_t index);

  virtual bool isPieceUsed(size_t index);

  virtual uint64_t getTotalLength()
  {
    return _totalLength;
  }

  virtual uint64_t getFilteredTotalLength()
  {
    return _totalLength;
  }

  virtual uint64_t getCompletedLength()
  {
    // TODO we have to return actual completed length here?
    return _totalLength;
  }

  virtual uint64_t getFilteredCompletedLength()
  {
    return getCompletedLength();
  }
  
  virtual void setFileFilter(const std::deque<std::string>& filePaths) {}

  virtual void setFileFilter(IntSequence seq) {}

  virtual void clearFileFilter() {}
  
  /**
   * Returns true if download has completed.
   * If file filter is enabled, then returns true if those files have
   * downloaded.
   */
  virtual bool downloadFinished()
  {
    return _downloadFinished;
  }

  /**
   * Returns true if all files have downloaded.
   * The file filter is ignored.
   */
  virtual bool allDownloadFinished()
  {
    return downloadFinished();
  }

  /**
   * Initializes DiskAdaptor.
   * TODO add better documentation here.
   */
  virtual void initStorage();

  virtual const unsigned char* getBitfield()
  {
    return 0;
  }

  virtual void setBitfield(const unsigned char* bitfield,
			   size_t bitfieldLength) {}
  
  virtual size_t getBitfieldLength()
  {
    return 0;
  }

  virtual bool isSelectiveDownloadingMode()
  {
    return false;
  }

  virtual void finishSelectiveDownloadingMode() {}

  virtual bool isEndGame()
  {
    return false;
  }

  virtual SharedHandle<DiskAdaptor> getDiskAdaptor();
  
  virtual size_t getPieceLength(size_t index);

  /**
   * Adds piece index to advertise to other commands. They send have message
   * based on this information.
   */
  virtual void advertisePiece(int32_t cuid, size_t index) {}

  /**
   * Returns piece index which is not advertised by the caller command and
   * newer than lastCheckTime.
   */
  virtual std::deque<size_t>
  getAdvertisedPieceIndexes(int32_t myCuid, const Time& lastCheckTime)
  {
    return std::deque<size_t>();
  }

  /**
   * Removes have entry if specified seconds have elapsed since its
   * registration.
   */
  virtual void removeAdvertisedPiece(time_t elapsed) {}

  /**
   * Sets all bits in bitfield to 1.
   */
  virtual void markAllPiecesDone();

  virtual void markPiecesDone(uint64_t length);

  virtual void markPieceMissing(size_t index);

  /**
   * Do nothing because loading in-flight piece is not supported for this
   * class.
   */
  virtual void addInFlightPiece(const std::deque<SharedHandle<Piece> >& pieces) {}

  virtual size_t countInFlightPiece()
  {
    return 0;
  }

  virtual std::deque<SharedHandle<Piece> > getInFlightPieces();

  virtual void addPieceStats(size_t index) {}

  virtual void addPieceStats(const unsigned char* bitfield,
			     size_t bitfieldLength) {}

  virtual void subtractPieceStats(const unsigned char* bitfield,
				  size_t bitfieldLength) {}

  virtual void updatePieceStats(const unsigned char* newBitfield,
				size_t newBitfieldLength,
				const unsigned char* oldBitfield) {}


  void setDiskWriterFactory(const SharedHandle<DiskWriterFactory>& diskWriterFactory);
};

typedef SharedHandle<UnknownLengthPieceStorage> UnknownLengthPieceStorageHandle;

} // namespace aria2

#endif // _D_UNKNOWN_LENGTH_PIECE_STORAGE_H_
