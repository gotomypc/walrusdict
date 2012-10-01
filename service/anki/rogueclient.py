#!/usr/bin/python
# coding: utf-8

import os, sys, optparse, traceback

from aqt import *
from aqt.profiles import ProfileManager

from anki.errors import *
from anki import Collection
from anki.models import ModelManager
from anki.utils import intTime
from anki.sync import Syncer, FullSyncer, RemoteServer
from anki.notes import Note

DEBUG = True

class RogueAnki:

    def __init__(self, base, profile):
        self.base = unicode(base or "", sys.getfilesystemencoding())
        self.profile = unicode(profile or "", sys.getfilesystemencoding())

        self.pm = ProfileManager(self.base, self.profile)

        if self.profile:
            self.pm.ensureProfile()
        else:
            # defaulting to first frofile found
            name = self.pm.profiles()[0];
            self.pm = ProfileManager(self.base, name)
            self.pm.ensureProfile()

        self.collection = Collection(self.pm.collectionPath())

    def add(self, front, back, deck_name, model_name):

        default_deck = self.collection.decks.allNames()[0]
        default_model = self.collection.models.allNames()[0]
        
        deck_name = unicode(deck_name or default_deck, sys.getfilesystemencoding())
        model_name = unicode(model_name or default_model, sys.getfilesystemencoding())

        model = self.collection.models.byName(model_name)
        if not model:
            mm = ModelManager(self.collection)
            model = mm.new(model_name)
            mm.save(model)

        model['did'] = did = self.collection.decks.id(deck_name)
        self.collection.models.save()
        
        note = self.collection.newNote()
        note['Front']= unicode(front or "", sys.getfilesystemencoding())
        note['Back']= unicode(back or "", sys.getfilesystemencoding())
        note.model()['did']= did
        self.collection.addNote(note)
        self.collection.save()
        self.collection.genCards(self.collection.models.nids(model))

    def list_decks(self):
        print "decks:"
        for x in self.collection.decks.allNames():
            print x
        print "models:"
        for x in self.collection.models.allNames():
            print x

    def sync(self):
        server= RemoteServer(self.pm.profile['syncKey'])
        client = Syncer(self.collection, server)
        ret = None
        try:
            print client.sync()
        except Exception, e:
            log = traceback.format_exc()
            try:
                err = unicode(e[0], "utf8", "ignore")
            except:
                # number, exception with no args, etc
                err = ""
            if "Unable to find the server" in err:
                print "offline"
                return
            else:
                if not isinstance(log, unicode):
                    err = unicode(log, "utf8", "replace")
                    print "error", log
            return
        if ret == "badAuth":
            print "badAuth"
            return
        elif ret == "clockOff":
            print "clockOff"
            return
        if ret == "fullSync":
            print "fullSync"
            return self.fullSync(server)
        # save and note success state
        self.collection.save()
        if ret == "noChanges":
            print "noChanges"
        else:
            print "success"
                        
    def _fullSync(self, server):
        # if the local deck is empty, assume user is trying to download
        if self.collection.isEmpty():        
            f = "download"
        else:
            # tell the calling thread we need a decision on sync direction, and
            # wait for a reply
            f = "cancel"
            print "decision needed"

        print f
        if f == "cancel":
            return
        client = FullSyncer(self.collection, self.pm.profile['syncKey'], server.con)
        if f == "upload":
            client.upload()
        else:
            client.download()
        # reopen db and move on to media sync


def parseArgs(argv):
    "Returns (opts, args)."
    parser = optparse.OptionParser()
    parser.usage = "%prog [OPTIONS] [file to import]"
    parser.add_option("-F", "--front", help="note front")
    parser.add_option("-B", "--back", help="note back")
    parser.add_option("-D", "--deck", help="deck to use")
    parser.add_option("-M", "--model", help="model to use")
    parser.add_option("-s", action="store_true", dest="sync")
    parser.add_option("-l", action="store_true", dest="list")
    parser.add_option("-b", "--base", help="path to base folder")
    parser.add_option("-p", "--profile", help="profile name to load")

    return parser.parse_args(argv[1:])

opts, args = parseArgs(sys.argv)

ranki = RogueAnki(opts.base, opts.profile)

if (opts.list):
    ranki.list_decks()
    exit()

if (opts.model or opts.deck):
    # default: model = deck
    opts.model = opts.model or opts.deck
    opts.deck = opts.deck or opts.model
else:
    print "deck or model must be specified"
    exit()

# presync
if(opts.sync):
    ranki.sync()

if(opts.front and opts.back):
    ranki.add(opts.front, opts.back, opts.deck, opts.model)
else:
    print "not added"

# postsync
if(opts.sync):
    ranki.sync()

