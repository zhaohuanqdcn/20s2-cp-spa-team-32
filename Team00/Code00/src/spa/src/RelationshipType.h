#pragma once

enum RelationshipType {
	NONE = 0,
	FOLLOWS = 1,
	FOLLOWS_T = 2,
	PARENT = 3,
	PARENT_T = 4,
	USES = 5,
	MODIFIES = 6,
	CALLS = 7,
	CALLS_T = 8,
	NEXT = 9,
	NEXT_T = 10,
	AFFECTS = 11,
	AFFECTS_T = 12,
	NEXTBIP = 13,
	NEXTBIP_T = 14,
	AFFECTSBIP = 15,
	AFFECTSBIP_T = 16
};

