// Scintilla source code edit control
/** @file CellBuffer.h
 ** Manages the text of the document.
 **/
// Copyright 1998-2004 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef CELLBUFFER_H
#define CELLBUFFER_H

/* C::B begin */
#include "RunStyles.h"
/* C::B end */

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

// Interface to per-line data that wants to see each line insertion and deletion
class PerLine {
public:
	virtual ~PerLine() {}
	virtual void Init()=0;
	virtual void InsertLine(int)=0;
	virtual void RemoveLine(int)=0;
};

/* CHANGEBAR begin */
#include "RunStyles.h"
class LineChanges {
    bool collecting;
    RunStyles state;
    int edition;
public:
    LineChanges();
    ~LineChanges();
    void AdvanceEdition();
    int GetEdition() const;
    char *PersistantForm() const;
    void SetChanges(const char *changesState);
    void InsertText(int line, int edition, bool undoing);
    void InsertLine(int line, int edition, bool undoing);
    void RemoveLine(int line, bool undoing);
    void EnableChangeCollection(bool collecting_, int lines);
    void ClearChanged();
    int GetChanged(int line) const;
};
/* CHANGEBAR end */


/**
 * The line vector contains information about each of the lines in a cell buffer.
 */
class LineVector {

	Partitioning starts;
	PerLine *perLine;
/* CHANGEBAR begin */
    LineChanges changes;
/* CHANGEBAR end */

public:

	LineVector();
	~LineVector();
	void Init();
	void SetPerLine(PerLine *pl);

/* CHANGEBAR begin */
	void InsertText(int line, int delta, int edition, bool undoing, bool lineUnchanged);
    void InsertLine(int line, int position, bool lineStart, int edition, bool undoing);
/* CHANGEBAR end */
	void SetLineStart(int line, int position);
/* CHANGEBAR begin */
    void RemoveLine(int line, bool undoing);
/* CHANGEBAR end */
	int Lines() const {
		return starts.Partitions();
	}
	int LineFromPosition(int pos) const;
	int LineStart(int line) const {
		return starts.PositionFromPartition(line);
	}

	int MarkValue(int line);
	int AddMark(int line, int marker);
	void MergeMarkers(int pos);
	void DeleteMark(int line, int markerNum, bool all);
	void DeleteMarkFromHandle(int markerHandle);
	int LineFromHandle(int markerHandle);

	void ClearLevels();
	int SetLevel(int line, int level);
	int GetLevel(int line);

	int SetLineState(int line, int state);
	int GetLineState(int line);
	int GetMaxLineState();


/* CHANGEBAR begin */
    void EnableChangeCollection(bool changesCollecting_);
    void DeleteChangeCollection();
    int GetChanged(int line) const;
    void SetSavePoint();
    int GetChangesEdition() const;
    void PerformingUndo(bool undo);
    char *PersistantForm() const;
    void SetChanges(const char *changesState);
/* CHANGEBAR end */
};

enum actionType { insertAction, removeAction, startAction, containerAction };

/**
 * Actions are used to store all the information required to perform one undo/redo step.
 */
class Action {
public:
	actionType at;
	int position;
	char *data;
	int lenData;
	bool mayCoalesce;

	Action();
	~Action();
	void Create(actionType at_, int position_=0, char *data_=0, int lenData_=0, bool mayCoalesce_=true);
	void Destroy();
	void Grab(Action *source);
};

/**
 *
 */
class UndoHistory {
	Action *actions;
	int lenActions;
	int maxAction;
	int currentAction;
	int undoSequenceDepth;
	int savePoint;
/* CHANGEBAR begin */
    int savePointEffective;
    int **changeActions;
/* CHANGEBAR end */

	void EnsureUndoRoom();

public:
	UndoHistory();
	~UndoHistory();

/* CHANGEBAR begin */
	void AppendAction(actionType at, int position, char *data, int length, bool &startSequence, char *persistantChanges, bool mayCoalesce=true);
/* CHANGEBAR end */

	void BeginUndoAction();
	void EndUndoAction();
	void DropUndoSequence();
	void DeleteUndoHistory();

/* CHANGEBAR begin */
    void DeleteChangeHistory();
    void EnableChangeHistory(bool enable);
/* CHANGEBAR end */

	/// The save point is a marker in the undo stack where the container has stated that
	/// the buffer was saved. Undo and redo can move over the save point.
	void SetSavePoint();
	bool IsSavePoint() const;
/* CHANGEBAR begin */
    bool BeforeSavePointEffective(int action) const;
/* CHANGEBAR end */

	/// To perform an undo, StartUndo is called to retrieve the number of steps, then UndoStep is
	/// called that many times. Similarly for redo.
	bool CanUndo() const;
	int StartUndo();
	const Action &GetUndoStep() const;
	void CompletedUndoStep();
/* CHANGEBAR begin */
    char *GetChangesStep() const;
/* CHANGEBAR end */
	bool CanRedo() const;
	int StartRedo();
	const Action &GetRedoStep() const;
	void CompletedRedoStep();

/* CHANGEBAR begin */
    int Edition() const;
/* CHANGEBAR end */
};

/**
 * Holder for an expandable array of characters that supports undo and line markers.
 * Based on article "Data Structures in a Bit-Mapped Text Editor"
 * by Wilfred J. Hansen, Byte January 1987, page 183.
 */
class CellBuffer {
private:
	SplitVector<char> substance;
	SplitVector<char> style;
	bool readOnly;

	bool collectingUndo;
	UndoHistory uh;

	LineVector lv;

	/// Actions without undo
/* CHANGEBAR begin */
    void BasicInsertString(int position, const char *s, int insertLength, bool undoing);
    void BasicDeleteChars(int position, int deleteLength, bool undoing);
/* CHANGEBAR end */

public:

	CellBuffer();
	~CellBuffer();

	/// Retrieving positions outside the range of the buffer works and returns 0
	char CharAt(int position) const;
	void GetCharRange(char *buffer, int position, int lengthRetrieve) const;
	char StyleAt(int position) const;
	void GetStyleRange(unsigned char *buffer, int position, int lengthRetrieve) const;
	const char *BufferPointer();
	const char *RangePointer(int position, int rangeLength);
	int GapPosition() const;

	int Length() const;
	void Allocate(int newSize);
	void SetPerLine(PerLine *pl);
	int Lines() const;
	int LineStart(int line) const;
	int LineFromPosition(int pos) const { return lv.LineFromPosition(pos); }
/* CHANGEBAR begin */
    void InsertLine(int line, int position, bool lineStart, int edition, bool undoing);
    void RemoveLine(int line, bool undoing);
/* CHANGEBAR end */
	const char *InsertString(int position, const char *s, int insertLength, bool &startSequence);

	/// Setting styles for positions outside the range of the buffer is safe and has no effect.
	/// @return true if the style of a character is changed.
	bool SetStyleAt(int position, char styleValue, char mask='\377');
	bool SetStyleFor(int position, int length, char styleValue, char mask);

	const char *DeleteChars(int position, int deleteLength, bool &startSequence);

	bool IsReadOnly() const;
	void SetReadOnly(bool set);

	/// The save point is a marker in the undo stack where the container has stated that
	/// the buffer was saved. Undo and redo can move over the save point.
	void SetSavePoint();
	bool IsSavePoint();

/* CHANGEBAR begin */
    void EnableChangeCollection(bool changesCollecting_);
    bool SetChangeCollection(bool collectChange);
    void DeleteChangeCollection();
    int GetChanged(int line) const;
    int GetChangesEdition() const;
/* CHANGEBAR end */

	bool SetUndoCollection(bool collectUndo);
	bool IsCollectingUndo() const;
	void BeginUndoAction();
	void EndUndoAction();
	void AddUndoAction(int token, bool mayCoalesce);
/* CHANGEBAR begin */
    void DeleteUndoHistory(bool collectChangeHistory);
/* CHANGEBAR end */

	/// To perform an undo, StartUndo is called to retrieve the number of steps, then UndoStep is
	/// called that many times. Similarly for redo.
	bool CanUndo();
	int StartUndo();
	const Action &GetUndoStep() const;
	void PerformUndoStep();
	bool CanRedo();
	int StartRedo();
	const Action &GetRedoStep() const;
	void PerformRedoStep();
};

#ifdef SCI_NAMESPACE
}
#endif

#endif
