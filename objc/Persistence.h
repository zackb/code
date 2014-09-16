//  Adds data translation utilities for any object. Translate
//  an object to and from an NSDictionary.
//
//  Created by Zack Bartel on 9/27/13.
//  Copyright (c) 2013 Zack Bartel. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol Persistable

@optional

+ (NSDictionary *)collectionTypes;
+ (NSDictionary *)propertyOverrides;

@end


@interface NSObject (Persistence)

+ (NSArray *)properties;

- (NSDictionary *)dictionary;
+ (id)withDictionary:(NSDictionary *)dictionary;

- (void)save;
- (void)save:(NSString *)path;
+ (id)get;
+ (id)load:(NSString *)path;

@end
