//
// CXDocumentController.m
// Classix
//
// Copyright (C) 2012 Félix Cloutier
//
// This file is part of Classix.
//
// Classix is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// Classix is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Classix. If not, see http://www.gnu.org/licenses/.
//

#import "CXDocumentController.h"

@implementation CXDocumentController

+(CXDocumentController*)documentController
{
	return (CXDocumentController*)[NSDocumentController sharedDocumentController];
}

-(id)init
{
	if (!(self = [super init]))
		return nil;
	
	docToId = [NSMapTable weakToWeakObjectsMapTable];
	idToDoc = [NSMapTable weakToWeakObjectsMapTable];
	
	return self;
}

-(CXDocument*)documentWithId:(NSUInteger)docId
{
	return [idToDoc objectForKey:@(docId)];
}

-(NSUInteger)idOfDocument:(CXDocument *)document
{
	return [[docToId objectForKey:document] unsignedIntegerValue];
}

-(void)addDocument:(NSDocument *)document
{
	NSUInteger documentId = docToId.count + 1;
	NSNumber* key = @(documentId);
	[docToId setObject:key forKey:document];
	[idToDoc setObject:document forKey:key];
	[super addDocument:document];
}

-(void)removeDocument:(NSDocument *)document
{
	NSUInteger documentId = [[docToId objectForKey:document] unsignedIntegerValue];
	[docToId removeObjectForKey:document];
	[idToDoc removeObjectForKey:@(documentId)];
	[super removeDocument:document];
}

-(NSString*)defaultType
{
	// this ensures that the app won't try to create a new untitled document
	return nil;
}

-(void)dealloc
{
	[docToId release];
	[idToDoc release];
	[super dealloc];
}

@end
