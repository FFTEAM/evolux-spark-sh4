/************************************************************************
COPYRIGHT (C) STMicroelectronics R&D Ltd 2007

Source file name : frame_parser_video_theora.h
Author :           Julian

Definition of the frame parser video theora class implementation for player 2.


Date        Modification                                    Name
----        ------------                                    --------
10-Mar-09   Created                                         Julian

************************************************************************/

#ifndef H_FRAME_PARSER_VIDEO_THEORA
#define H_FRAME_PARSER_VIDEO_THEORA

// /////////////////////////////////////////////////////////////////////
//
//      Include any component headers

#include "theora.h"
#include "frame_parser_video.h"

// /////////////////////////////////////////////////////////////////////////
//
// Locally defined structures
//

#define THEORA_REFERENCE_FRAMES_FOR_FORWARD_DECODE              1

typedef enum
{
    TheoraNoHeaders                     = 0x00,
    TheoraIdentificationHeader          = 0x01,
    TheoraCommentHeader                 = 0x02,
    TheoraSetupHeader                   = 0x04,
    TheoraAllHeaders                    = 0x07,
} TheoraHeader_t;



// /////////////////////////////////////////////////////////////////////////
//
// The class definition
//

/// Frame parser for Theora
class FrameParser_VideoTheora_c : public FrameParser_Video_c
{
private:

    TheoraStreamParameters_t*   StreamParameters;
    TheoraFrameParameters_t*    FrameParameters;
    unsigned int                StreamHeadersRead;

    Rational_t                  FrameRate;
    Rational_t                  PixelAspectRatio;

    FrameParserStatus_t         ReadStreamHeaders(                              void );
    FrameParserStatus_t         ReadPictureHeader(                              void );
    int                         ReadHuffmanTree(                                TheoraVideoSequence_t*  SequenceHeader);

    FrameParserStatus_t         CommitFrameForDecode(                           void );
    bool                        NewStreamParametersCheck(                       void );

public:

    FrameParser_VideoTheora_c(                                                  void );
    ~FrameParser_VideoTheora_c(                                                 void );

    //
    // Overrides for component base class functions
    //

    FrameParserStatus_t         Reset(                                          void );

    //
    // FrameParser class functions
    //

    FrameParserStatus_t         RegisterOutputBufferRing(                       Ring_t          Ring );

    //
    // Stream specific functions
    //

    FrameParserStatus_t         ReadHeaders(                                    void );

    FrameParserStatus_t         PrepareReferenceFrameList(                      void );

    FrameParserStatus_t         ForPlayUpdateReferenceFrameList(                void );

    FrameParserStatus_t         RevPlayProcessDecodeStacks(                     void );
    FrameParserStatus_t         RevPlayGeneratePostDecodeParameterSettings(     void );
    FrameParserStatus_t         RevPlayRemoveReferenceFrameFromList(            void );
};

#endif

