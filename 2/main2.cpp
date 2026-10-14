#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <iomanip>

class FootballClub {
    std::string m_name;
    std::string m_city;
    int m_foundationYear;
    int m_rating; // 1 to 5

public:
    FootballClub(const std::string& name, const std::string& city, int year, int rating)
        : m_name(name), m_city(city), m_foundationYear(year), m_rating(rating) {
        if (m_rating < 1) m_rating = 1;
        if (m_rating > 5) m_rating = 5;
    }

    const std::string& getName() const { return m_name; }
    const std::string& getCity() const { return m_city; }
    int getFoundationYear() const { return m_foundationYear; }
    int getRating() const { return m_rating; }
};


class Match {
    FootballClub m_homeClub;
    FootballClub m_awayClub;
    int m_homeGoals;
    int m_awayGoals;
    bool m_isPlayed;

public:
    Match(const FootballClub& home, const FootballClub& away)
        : m_homeClub(home), m_awayClub(away), m_homeGoals(0), m_awayGoals(0), m_isPlayed(false) {
    }


    void setScore(int homeGoals, int awayGoals) {
        m_homeGoals = homeGoals;
        m_awayGoals = awayGoals;
        m_isPlayed = true;
    }


    void generateRandomScore() {
        static std::random_device rd;
        static std::mt19937 gen(rd());

        double homeLambda = 1.0 + (m_homeClub.getRating() * 0.4);
        double awayLambda = 0.8 + (m_awayClub.getRating() * 0.4);

        std::poisson_distribution<int> homeDist(homeLambda);
        std::poisson_distribution<int> awayDist(awayLambda);

        m_homeGoals = homeDist(gen);
        m_awayGoals = awayDist(gen);
        m_isPlayed = true;
    }

    const FootballClub& getHomeClub() const { return m_homeClub; }
    const FootballClub& getAwayClub() const { return m_awayClub; }
    int getHomeGoals() const { return m_homeGoals; }
    int getAwayGoals() const { return m_awayGoals; }
    bool isPlayed() const { return m_isPlayed; }

    void print() const {
        std::cout << std::left << std::setw(15) << m_homeClub.getName()
            << " " << m_homeGoals << " : " << m_awayGoals << " "
            << std::right << std::setw(15) << m_awayClub.getName() << "\n";
    }
};

struct ClubStats {
    std::string clubName;
    int points = 0;
    int wins = 0;
    int draws = 0;
    int losses = 0;
    int goalsScored = 0;
    int goalsConceded = 0;

    int getGoalDifference() const {
        return goalsScored - goalsConceded;
    }
};

class TournamentTable {
    std::vector<ClubStats> m_stats;

public:
    TournamentTable() = default;

    void update(const std::vector<FootballClub>& clubs, const std::vector<Match>& matches) {
        m_stats.clear();
        for (const auto& club : clubs) {
            ClubStats stat;
            stat.clubName = club.getName();
            m_stats.push_back(stat);
        }

        for (const auto& match : matches) {
            if (!match.isPlayed()) continue;

            auto homeIt = std::find_if(m_stats.begin(), m_stats.end(),
                [&](const ClubStats& s) { return s.clubName == match.getHomeClub().getName(); });
            auto awayIt = std::find_if(m_stats.begin(), m_stats.end(),
                [&](const ClubStats& s) { return s.clubName == match.getAwayClub().getName(); });

            if (homeIt != m_stats.end() && awayIt != m_stats.end()) {
                int hG = match.getHomeGoals();
                int aG = match.getAwayGoals();

                homeIt->goalsScored += hG;
                homeIt->goalsConceded += aG;
                awayIt->goalsScored += aG;
                awayIt->goalsConceded += hG;

                if (hG > aG) {
                    homeIt->points += 3;
                    homeIt->wins++;
                    awayIt->losses++;
                }
                else if (hG < aG) {
                    awayIt->points += 3;
                    awayIt->wins++;
                    homeIt->losses++;
                }
                else {
                    homeIt->points += 1;
                    awayIt->points += 1;
                    homeIt->draws++;
                    awayIt->draws++;
                }
            }
        }

        std::sort(m_stats.begin(), m_stats.end(), [](const ClubStats& a, const ClubStats& b) {
            if (a.points != b.points) return a.points > b.points;
            if (a.getGoalDifference() != b.getGoalDifference()) return a.getGoalDifference() > b.getGoalDifference();
            return a.goalsScored > b.goalsScored;
            });
    }

    void print(std::ostream& os) const {
        os << std::left << std::setw(6) << "Pos"
            << std::setw(20) << "Club"
            << std::setw(8) << "Pts"
            << std::setw(6) << "W"
            << std::setw(6) << "D"
            << std::setw(6) << "L"
            << std::setw(10) << "Goals" << "\n";
        os << std::string(62, '-') << "\n";

        for (size_t i = 0; i < m_stats.size(); ++i) {
            std::string goalsStr = std::to_string(m_stats[i].goalsScored) + ":" + std::to_string(m_stats[i].goalsConceded);
            os << std::left << std::setw(6) << (i + 1)
                << std::setw(20) << m_stats[i].clubName
                << std::setw(8) << m_stats[i].points
                << std::setw(6) << m_stats[i].wins
                << std::setw(6) << m_stats[i].draws
                << std::setw(6) << m_stats[i].losses
                << std::setw(10) << goalsStr << "\n";
        }
    }

    const std::vector<ClubStats>& getStats() const { return m_stats; }
};


class Tournament {
    std::string m_name;
    std::vector<FootballClub> m_clubs;
    std::vector<Match> m_matches;
    TournamentTable m_table;

public:
    Tournament(const std::string& name, const std::vector<FootballClub>& clubs)
        : m_name(name), m_clubs(clubs) {
    }

    void generateSchedule() {
        m_matches.clear();
        for (size_t i = 0; i < m_clubs.size(); ++i) {
            for (size_t j = i + 1; j < m_clubs.size(); ++j) {
                m_matches.emplace_back(m_clubs[i], m_clubs[j]);
            }
        }
    }

    void simulateMatches() {
        for (auto& match : m_matches) {
            match.generateRandomScore();
        }
        updateTable();
    }

    void updateTable() {
        m_table.update(m_clubs, m_matches);
    }

    void printMatches() const {
        std::cout << "\n=== Match Results for \"" << m_name << "\" ===\n";
        for (const auto& match : m_matches) {
            match.print();
        }
    }

    void printTable(std::ostream& os) const {
        os << "\n=== Standings Table for \"" << m_name << "\" ===\n";
        m_table.print(os);
    }

    std::string getWinner() const {
        const auto& stats = m_table.getStats();
        if (!stats.empty()) {
            return stats.front().clubName;
        }
        return "Tournament not played";
    }

    std::vector<std::string> getLastTwoClubs() const {
        std::vector<std::string> result;
        const auto& stats = m_table.getStats();
        if (stats.size() >= 2) {
            result.push_back(stats[stats.size() - 2].clubName);
            result.push_back(stats.back().clubName);
        }
        return result;
    }
};

int main() {

    std::vector<FootballClub> clubs = {
        FootballClub("Dynamo", "Kyiv", 1927, 5),
        FootballClub("Shakhtar", "Donetsk", 1936, 5),
        FootballClub("Zorya", "Luhansk", 1923, 3),
        FootballClub("Vorskla", "Poltava", 1955, 3)
    };

    Tournament tournament("Premier League", clubs);

    tournament.generateSchedule();

    tournament.simulateMatches();

    tournament.printMatches();

    tournament.printTable(std::cout);

    std::cout << "\nTournament Winner: " << tournament.getWinner() << "\n";

    auto lastClubs = tournament.getLastTwoClubs();
    if (lastClubs.size() == 2) {
        std::cout << "Clubs in bottom two places: "
            << lastClubs[0] << " and " << lastClubs[1] << "\n";
    }

    return 0;
}