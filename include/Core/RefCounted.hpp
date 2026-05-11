#pragma once

/*
    Class parent for reference counted objects
*/


class RefCounted
{


private:
    unsigned int m_references;
};