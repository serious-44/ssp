/**
 * Copyright (c) 2012 Andrew Prock. All rights reserved.
 * $Id: PokerHandEvaluator_Alloc.cpp 2649 2012-06-30 04:53:24Z prock $
 */
#include <stdexcept>
#include "HoldemHandEvaluator.h"

using namespace std;
using namespace pokerstove;

std::shared_ptr<PokerHandEvaluator> PokerHandEvaluator::alloc(const string& strid)
{
    std::shared_ptr<PokerHandEvaluator> ret;
    switch (strid[0])
    {
        case 'h':       //     hold'em
        case 'H':       //     hold'em
            ret.reset(new HoldemHandEvaluator);
            break;
    }

    ret->_subclassID = strid;

    return ret;
}
