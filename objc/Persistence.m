//  Adds data translation utilities for any object. Translate
//  an object to and from an NSDictionary.
//
//  Created by Zack Bartel on 9/27/13.
//  Copyright (c) 2013 Zack Bartel. All rights reserved.
//

#import "Persistence.h"
#import <objc/runtime.h>

@implementation NSObject (Persistence)

+ (NSArray *)properties {
    
    static NSMutableDictionary *_prop_cache = nil;
    if (!_prop_cache) {
        _prop_cache = [NSMutableDictionary dictionary];
    }
    
    NSString *clasName = NSStringFromClass([self class]);
    if (_prop_cache[clasName]) {
        return _prop_cache[clasName];
    }
    
    NSMutableArray *props = [NSMutableArray array];
    unsigned int count, i;
    objc_property_t *properties = class_copyPropertyList([self class], &count);
    
    for (i = 0; i < count; i++) {
        objc_property_t property = properties[i];
        const char *name = property_getName(property);
        NSString *sname = [NSString stringWithUTF8String:name];
        
        [props addObject:sname];
    }
    
    free(properties);
    
    _prop_cache[clasName] = props;
    
    return props;
}

- (NSDictionary *)dictionary {
    
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    
    for (id property in [[self class] properties]) {
        id value = [self valueForKey:property];
        NSString *className = NSStringFromClass([value class]);
        if (value) {
            if ([value isKindOfClass:[NSArray class]]) {
                NSMutableArray *values = [NSMutableArray array];
                for (id subValue in value) {
                    if (subValue && subValue != [NSNull null]) {
                        [values addObject:[subValue dictionary]];
                    }
                }
                value = [values count] > 0 ? values : nil;
            } else if (![className hasPrefix:@"__NS"] && ![className hasPrefix:@"NS"]) {
                value = [value dictionary];
            }
            if (value && value != [NSNull null]) {
                dict[property] = value;
            }
        }
    }
    
    return dict;
}

+ (id)withDictionary:(NSDictionary *)dictionary {
    
    Class class = [self class];
    id obj = [[class alloc] init];
    
    NSDictionary *collectionTypes = @{};
    NSDictionary *propertyOverrides = @{};
    
    if ([class conformsToProtocol:@protocol(Persistable)]) {
        if ([class respondsToSelector:@selector(collectionTypes)]) {
            collectionTypes = [(Class<Persistable>)[obj class] collectionTypes];
        }
        if ([class respondsToSelector:@selector(propertyOverrides)]) {
            propertyOverrides = [(Class<Persistable>)[obj class] propertyOverrides];
        }
    }
    
    NSArray *props = [class properties];
    for (id key in dictionary) {
        id prop = propertyOverrides[key] ?: key;
        if ([props containsObject:prop]) {
            Class collectionClass = collectionTypes[prop];
            if (collectionTypes[prop]) {
                NSMutableArray *collection = [NSMutableArray array];
                for (id map in dictionary[key]) {
                    id subObj = [collectionClass withDictionary:map];
                    [collection addObject:subObj];
                }
                [obj setValue:collection forKey:prop];
            } else {
                objc_property_t property = class_getProperty(class, [prop UTF8String]);
                Class type = typeOf(property);
                id value = convertValue(type, dictionary[key]);
                if (value && value != [NSNull null]) {
                    [obj setValue:value forKey:prop];
                }
            }
        }
    }
    
    return obj;
}

- (void)save {
    
    NSString *filename = [NSStringFromClass([self class]) stringByAppendingString:@".plist"];
    [self save:filename];
}

- (void)save:(NSString *)filename {
    
    NSString *path = [documentRoot() stringByAppendingPathComponent:filename];
    [[self dictionary] writeToFile:path atomically:YES];
}

+ (id)get {
    
    NSString *filename = [NSStringFromClass([self class]) stringByAppendingString:@".plist"];
    return [self load:filename];
}

+ (id)load:(NSString *)filename {
    
    NSString *path = [documentRoot() stringByAppendingPathComponent:filename];
    if (![[NSFileManager defaultManager] fileExistsAtPath:filename]) {
        return nil;
    }
    id dict = [NSDictionary dictionaryWithContentsOfFile:path];
    return [[self class] withDictionary:dict];
}

NSString *documentRoot() {
    static NSString *path;
    if (!path) {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        path = [paths objectAtIndex:0];
    }
    
    return path;
}

Class typeOf(objc_property_t property) {
    
	const char *attrs = property_getAttributes(property);
	if (attrs == NULL)
		return NULL;
    
	char buffer[256];
	const char *e = strchr(attrs, ',');
	if (e == NULL)
		return NULL;
    
	int len = (int)(e - attrs);
	memcpy(buffer, attrs, len);
	buffer[len] = '\0';
    
    NSString *typeString = [NSString stringWithUTF8String:buffer];
    NSString *className = [typeString substringWithRange:NSMakeRange(3, [typeString length] - 4)];
    
    return NSClassFromString(className);
}

id convertValue(Class type, id value) {
    
    id result = value;
    
    if (result == nil) {
        return result;
    }
    
    NSString *className = NSStringFromClass(type);
    
    if (type == [NSDate class] && ![value isKindOfClass:[NSDate class]]) {
        static NSDateFormatter *formatter = nil;
        if (!formatter) {
            formatter = [[NSDateFormatter alloc] init];
        }
        //NSLocale *en_US = [[NSLocale alloc] withLocaleIdentifier:@"en_US"];
        //[formatter setLocale:en_US];
        //2010-11-06T00:00:58Z
        [formatter setDateFormat:@"yyyy-MM-dd'T'HH:mm:ssZ"];
        [formatter setTimeZone:[NSTimeZone timeZoneForSecondsFromGMT:-60*60*7]];
        result = [formatter dateFromString:value];
    } else if (![className hasPrefix:@"__NS"] && ![className hasPrefix:@"NS"]) {
        result = [type withDictionary:value];
    }
    
    return result;
}

@end
