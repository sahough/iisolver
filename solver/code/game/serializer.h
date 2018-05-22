#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "../trainer/all.h"
#include "../poker/pocket.h"

#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/map.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

namespace boost {
namespace serialization {

//TREE CLASSES
template<class Archive>
void serialize(Archive & ar, SubHistory & sh, const unsigned int version)
{
        ar & sh.line;
        ar & sh.structure;
}

template<class Archive>
void serialize(Archive & ar, History & h, const unsigned int version)
{
        ar & h.streets;
        ar & h.board;
}

template<class Archive>
void serialize(Archive & ar, InfoSet & is, const unsigned int version)
{
        ar & is.handCode;
        ar & is.h;
}

template<class Archive>
void serialize(Archive & ar, Node & n, const unsigned int version)
{
        ar & n.ACTION_CT;
        ar & n.regretSum;
        ar & n.strategySum;
}

//POKER CLASSES (POCKET ONLY)
template<class Archive>
void serialize(Archive & ar, Pocket & p, const unsigned int version)
{
        ar & p.cards;
}

//TRAINER CLASSES
template<class Archive>
void serialize(Archive & ar, LookupTable & lt, const unsigned int version)
{
        ar & lt.F_SIZE;
        ar & lt.T_SIZE;
        ar & lt.R_SIZE;
        ar & lt.C_SIZE;
        ar & lt.flops;
        ar & lt.turns;
        ar & lt.rivers;
        ar & lt.range;
}

//Trainer itself is not being serialzed
/*
template<class Archive>
void serialize(Archive & ar, Trainer & tr, const unsigned int version)
{
        ar & tr.F_SIZE;
        ar & tr.T_SIZE;
        ar & tr.R_SIZE;
        ar & tr.C_SIZE;
        ar & tr.lt;
        ar & tr.nodeMap;
}
*/

} // namespace serialization
} // namespace boost

#endif
