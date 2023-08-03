#include "test.h"
#include "../res/testres.h"

// Dumb script to change all the text lines in a page
const char *LineModScript[] =
{
"\
setv p.name, \"Someone\"\n\
setv p.line[1], \"Line1\"\n\
setv p.line[2], \"Line2\"\n\
setv p.line[3], \"Line3\"\n\
setv p.line[4], \"Line4\"\n\
\0"
};

const VN_Character UnknownFemale =
{
    &img_FS_1,
    "Unknown Female"
};

const VN_Character CyberAnomalie =
{
    &img_FS_1,
    "Cyber anomalie"
};

const VN_Page Page0 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    &testBG2,           // BG
    &testFG2,           // FG

    TRUE,               // bTextbox
    (LFX_NONE),         // BG Effects
    (LFX_SHAKELR),      // FG Effects
    {
        "...\0",
        "oh hi there\0",
        "whats your name?\0",
        "\0"
    },
    0,                  // Switch delay
    {                   // Next Page
        &EntryPage,
        &Page2,
        NULL,
        NULL
    },
    LineModScript,      // Script
    NULL                // Track
};

const VN_Page Page1 =
{
    PAGETYPE_PAGE,      // Page Type
    &CyberAnomalie,     // Character

    &MD_BG0,            // BG
    &MD_FG1,            // FG

    TRUE,               // bTextbox
    (LFX_LINEGLITCH),   // BG Effects
    (LFX_LINEGLITCH),   // FG Effects
    {
        "Lorem ipsum dolor sit amet massa nun\0",
        "Aenean nec porta lectus. Sed non dui\0",
        "Ut consectetur scelerisque felis nis\0",
        "Sed ex elit, vestibulum et urna fusc\0"
    },
    0,                  // Switch delay
    {                   // Next Page
        &EntryPage,
        NULL,
        NULL,
        NULL
    },
    NULL,   // Script
    NULL    // Track
};

const VN_Page EntryPage =
{
    PAGETYPE_CHOICE,    // Page Type
    &UnknownFemale,     // Character

    &testBG2,           // BG
    &testFG2,           // FG

    TRUE,               // bTextbox
    (LFX_FADEIN | LFX_FADEOUT), // BG Effects
    (LFX_FADEIN | LFX_FADEOUT | LFX_SINEWAVE), // FG Effects
    {
        "Choice 1 - Script string test\0",
        "Choice 2 - error\0",
        "Choice 3 - FX Tests\0",
        "Choice 4 - This page\0"
    },
    0,                  // Switch delay
    {                   // Next Page
        &Page0,
        &Page1,
        &Page2,
        &EntryPage
    },
    NULL,   // Script
    NULL    // Track
};



// Fx tests

const VN_Page Page2 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    TRUE,               // bTextbox
    (LFX_FADEIN),      // BG Effects
    (LFX_NONE),         // FG Effects
    {
        "FadeIn BG\0",
        "\0",
        "\0",
        "\0"
    },
    0,                  // Switch delay
    {                   // Next Page
        &Page3,
        NULL,
        NULL,
        NULL
    },
    NULL,   // Script
    NULL    // Track
};
const VN_Page Page3 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    TRUE,               // bTextbox
    (LFX_NONE),      // BG Effects
    (LFX_FADEIN),         // FG Effects
    {
        "FadeIn FG\0",
        "\0",
        "\0",
        "\0"
    },
    0,                  // Switch delay
    {                   // Next Page
        &Page4,
        NULL,
        NULL,
        NULL
    },
    NULL,   // Script
    NULL    // Track
};

const VN_Page Page4 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    TRUE,               // bTextbox
    (LFX_FADEOUT),      // BG Effects
    (LFX_NONE),      // FG Effects
    {
        "FadeOut BG\0",
        "\0",
        "\0",
        "\0"
    },
    0,                  // Switch delay
    {                   // Next Page
        &Page5,
        NULL,
        NULL,
        NULL
    },
    NULL,   // Script
    NULL    // Track
};

const VN_Page Page5 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    TRUE,               // bTextbox
    (LFX_NONE),      // BG Effects
    (LFX_FADEOUT),      // FG Effects
    {
        "FadeOut FG\0",
        "\0",
        "\0",
        "\0"
    },
    0,                  // Switch delay
    {                   // Next Page
        &Page6,
        NULL,
        NULL,
        NULL
    },
    NULL,   // Script
    NULL    // Track
};

const VN_Page Page6 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    TRUE,               // bTextbox
    (LFX_SHAKELR),      // BG Effects
    (LFX_NONE),      // FG Effects
    {
        "ShakeLR BG\0",
        "\0",
        "\0",
        "\0"
    },
    0,                  // Switch delay
    {                   // Next Page
        &Page7,
        NULL,
        NULL,
        NULL
    },
    NULL,   // Script
    NULL    // Track
};

const VN_Page Page7 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    TRUE,               // bTextbox
    (LFX_NONE),      // BG Effects
    (LFX_SHAKELR),      // FG Effects
    {
        "ShakeLR FG\0",
        "\0",
        "\0",
        "\0"
    },
    0,                  // Switch delay
    {                   // Next Page
        &Page8,
        NULL,
        NULL,
        NULL
    },
    NULL,   // Script
    NULL    // Track
};

const VN_Page Page8 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    TRUE,               // bTextbox
    (LFX_LINEGLITCH),      // BG Effects
    (LFX_NONE),      // FG Effects
    {
        "LineGlitch BG\0",
        "\0",
        "\0",
        "\0"
    },
    0,                  // Switch delay
    {                   // Next Page
        &Page9,
        NULL,
        NULL,
        NULL
    },
    NULL,   // Script
    NULL    // Track
};

const VN_Page Page9 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    TRUE,               // bTextbox
    (LFX_NONE),      // BG Effects
    (LFX_LINEGLITCH),      // FG Effects
    {
        "LineGlitch FG\0",
        "\0",
        "\0",
        "\0"
    },
    0,                  // Switch delay
    {                   // Next Page
        &Page10,
        NULL,
        NULL,
        NULL
    },
    NULL,   // Script
    NULL    // Track
};

const VN_Page Page10 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    TRUE,               // bTextbox
    (LFX_NONE),      // BG Effects
    (LFX_SILHOUETTE),      // FG Effects
    {
        "Silhouette FG\0",
        "\0",
        "\0",
        "\0"
    },
    0,                  // Switch delay
    {                   // Next Page
        &Page11,
        NULL,
        NULL,
        NULL
    },
    NULL,   // Script
    NULL    // Track
};

const VN_Page Page11 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    TRUE,               // bTextbox
    (LFX_SINEWAVE),      // BG Effects
    (LFX_NONE),      // FG Effects
    {
        "SineWave BG\0",
        "\0",
        "\0",
        "\0"
    },
    0,                  // Switch delay
    {                   // Next Page
        &Page12,
        NULL,
        NULL,
        NULL
    },
    NULL,   // Script
    NULL    // Track
};

const VN_Page Page12 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    TRUE,               // bTextbox
    (LFX_NONE),      // BG Effects
    (LFX_SINEWAVE),      // FG Effects
    {
        "SineWave FG\0",
        "\0",
        "\0",
        "\0"
    },
    0,                  // Switch delay
    {                   // Next Page
        &EntryPage,
        NULL,
        NULL,
        NULL
    },
    NULL,   // Script
    NULL    // Track
};
